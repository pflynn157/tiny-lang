//
// Copyright 2021 Patrick Flynn
// This file is part of the Tiny Lang compiler.
// Tiny Lang is licensed under the BSD-3 license. See the COPYING file for more information.
//
#include <iostream>

#include <parser/Parser.hpp>

Parser::Parser(std::string input) {
    this->input = input;
    scanner = new Scanner(input);
    
    tree = new AstTree(input);
    syntax = new ErrorManager;
    
    // Add the built-in functions
    //string malloc(string)
    AstExternFunction *FT1 = new AstExternFunction("malloc");
    FT1->addArgument(Var(DataType::String));
    FT1->setDataType(DataType::String);
    tree->addGlobalStatement(FT1);
    
    //println(string)
    AstExternFunction *FT2 = new AstExternFunction("println");
    FT2->setVarArgs();
    FT2->addArgument(Var(DataType::String));
    FT2->setDataType(DataType::Void);
    tree->addGlobalStatement(FT2);
    
    //print(string)
    AstExternFunction *FT3 = new AstExternFunction("print");
    FT3->setVarArgs();
    FT3->addArgument(Var(DataType::String));
    FT3->setDataType(DataType::Void);
    tree->addGlobalStatement(FT3);
    
    //i32 strlen(string)
    AstExternFunction *FT4 = new AstExternFunction("strlen");
    FT4->addArgument(Var(DataType::String));
    FT4->setDataType(DataType::I32);
    tree->addGlobalStatement(FT4);
    
    //i32 stringcmp(string, string)
    AstExternFunction *FT5 = new AstExternFunction("stringcmp");
    FT5->addArgument(Var(DataType::String));
    FT5->addArgument(Var(DataType::String));
    FT5->setDataType(DataType::I32);
    tree->addGlobalStatement(FT5);
    
    //string strcat_str(string, string)
    AstExternFunction *FT6 = new AstExternFunction("strcat_str");
    FT6->addArgument(Var(DataType::String));
    FT6->addArgument(Var(DataType::String));
    FT6->setDataType(DataType::String);
    tree->addGlobalStatement(FT6);
    
    //string strcat_char(string, char)
    AstExternFunction *FT7 = new AstExternFunction("strcat_char");
    FT7->addArgument(Var(DataType::String));
    FT7->addArgument(Var(DataType::Char));
    FT7->setDataType(DataType::String);
    tree->addGlobalStatement(FT7);
}

Parser::~Parser() {
    delete scanner;
    delete syntax;
}

bool Parser::parse() {
    Token token;
    do {
        token = scanner->getNext();
        bool code = true;
        
        switch (token.type) {
            case Extern:
            case Func: {
                code = buildFunction(token);
            } break;
            
            case Const: code = buildConst(true); break;
            case Struct: code = buildStruct(); break;
            
            case Eof:
            case Nl: break;
            
            default: {
                syntax->addError(scanner->getLine(), "Invalid token in global scope.");
                token.print();
                code = false;
            }
        }
        
        if (!code) break;
    } while (token.type != Eof);
    
    // Check for errors, and print if so
    if (syntax->errorsPresent()) {
        syntax->printErrors();
        return false;
    }
    
    syntax->printWarnings();
    return true;
}

// Builds a statement block
bool Parser::buildBlock(AstBlock *block, int stopLayer, AstIfStmt *parentBlock, bool inElif) {
    Token token = scanner->getNext();
    while (token.type != Eof) {
        bool code = true;
        bool end = false;
        
        switch (token.type) {
            case VarD: code = buildVariableDec(block); break;
            case Struct: code = buildStructDec(block); break;
            case Const: code = buildConst(false); break;
            
            case Id: {
                Token idToken = token;
                token = scanner->getNext();
                
                if (token.type == Assign) {
                    code = buildVariableAssign(block, idToken);
                } else if (token.type == LBracket) {
                    code = buildArrayAssign(block, idToken);
                } else if (token.type == LParen) {
                    code = buildFunctionCallStmt(block, idToken);
                } else if (token.type == Dot) {
                    code = buildStructAssign(block, idToken);
                } else {
                    syntax->addError(scanner->getLine(), "Invalid use of identifier.");
                    token.print();
                    return false;
                }
            } break;
            
            case Return: code = buildReturn(block); break;
            
            // Handle conditionals
            // Yes, ELIF and ElSE are similar, but if you look closely, there is a subtle
            // difference (one very much needed)
            case If: code = buildConditional(block); break;
            case Elif: {
                if (inElif) {
                    scanner->rewind(token);
                    end = true;
                } else {
                    code = buildElif(parentBlock);
                }
            } break;
            case Else: {
                if (inElif) {
                    scanner->rewind(token);
                    end = true;
                } else {
                    code = buildElse(parentBlock);
                    end = true;
                }
            } break;
            
            // Handle loops
            case While: code = buildWhile(block); break;
            case Break: code = buildLoopCtrl(block, true); break;
            case Continue: code = buildLoopCtrl(block, false); break;
            
            // Handle the END keyword
            // This is kind of tricky in conditionals
            case End: {
                if (inElif) {
                    scanner->rewind(token);
                    end = true;
                    break;
                }
                if (layer == stopLayer) {
                    end = true;
                }
                if (layer > 0) --layer;
            } break;
            
            case Nl: break;
            
            default: {
                syntax->addError(scanner->getLine(), "Invalid token in expression.");
                token.print();
                return false;
            }
        }
        
        if (end) break;
        if (!code) return false;
        token = scanner->getNext();
    }
    
    return true;
}

// Builds an expression
bool Parser::buildExpression(AstStatement *stmt, DataType currentType, TokenType stopToken, TokenType separateToken,
                             AstExpression **dest, bool isConst) {
    std::stack<AstExpression *> output;
    std::stack<AstExpression *> opStack;
    int currentLine = scanner->getLine();
    
    DataType varType = currentType;
    
    bool lastWasOp = true;

    Token token = scanner->getNext();
    while (token.type != Eof && token.type != stopToken) {
        if (token.type == separateToken && output.size() > 0) {
            AstExpression *expr = output.top();
            output.pop();
            
            if (stmt == nullptr) {
                if ((*dest)->getType() == AstType::FuncCallExpr) {
                    AstFuncCallExpr *fc = static_cast<AstFuncCallExpr *>(*dest);
                    fc->addArgument(expr);
                } else {
                    *dest = expr;
                }
            } else {
                stmt->addExpression(expr);
            }
            continue;
        }
    
        switch (token.type) {
            case True: {
                lastWasOp = false;
                output.push(new AstI32(1));
            } break;
            
            case False: {
                lastWasOp = false;
                output.push(new AstI32(0));
            } break;
            
            case CharL: {
                lastWasOp = false;
                AstChar *c = new AstChar(token.i8_val);
                output.push(c);
            } break;
            
            case Int32: {
                lastWasOp = false;
                AstI32 *i32 = new AstI32(token.i32_val);
                output.push(i32);
            } break;
            
            case String: {
                lastWasOp = false;
                AstString *str = new AstString(token.id_val);
                output.push(str);
            } break;
            
            case Id: {
                lastWasOp = false;
            
                std::string name = token.id_val;
                if (varType == DataType::Void) {
                    varType = typeMap[name].first;
                    if (varType == DataType::Ptr) varType = typeMap[name].second;
                }
                
                token = scanner->getNext();
                if (token.type == LBracket) {
                    AstExpression *index = nullptr;
                    buildExpression(nullptr, DataType::I32, RBracket, EmptyToken, &index);
                    
                    AstArrayAccess *acc = new AstArrayAccess(name);
                    acc->setIndex(index);
                    output.push(acc);
                } else if (token.type == LParen) {
                    if (currentLine != scanner->getLine()) {
                        syntax->addWarning(scanner->getLine(), "Function call on newline- possible logic error.");
                    }
                
                    AstFuncCallExpr *fc = new AstFuncCallExpr(name);
                    AstExpression *fcExpr = fc;
                    buildExpression(nullptr, varType, RParen, Comma, &fcExpr);
                    
                    output.push(fc);
                } else if (token.type == Dot) {
                    // TODO: Search for structures here

                    Token idToken = scanner->getNext();
                    if (idToken.type != Id) {
                        syntax->addError(scanner->getLine(), "Expected identifier.");
                        return false;
                    }
                    
                    AstStructAccess *val = new AstStructAccess(name, idToken.id_val);
                    output.push(val);
                } else {
                    int constVal = isConstant(name);
                    if (constVal > 0) {
                        if (constVal == 1) {
                            AstExpression *expr = globalConsts[name].second;
                            output.push(expr);
                        } else if (constVal == 2) {
                            AstExpression *expr = localConsts[name].second;
                            output.push(expr);
                        }
                    } else {
                        AstID *id = new AstID(name);
                        output.push(id);
                    }
                    
                    scanner->rewind(token);
                }
            } break;
            
            case Plus: 
            case Minus:
            case And:
            case Or:
            case Xor: {
                if (opStack.size() > 0) {
                    AstType type = opStack.top()->getType();
                    if (type == AstType::Mul || type == AstType::Div) {
                        if (output.empty()) {
                            syntax->addError(scanner->getLine(), "Invalid expression: No RVAL");
                            return false;
                        }
                        AstExpression *rval = checkExpression(output.top(), varType);
                        output.pop();
                        
                        if (output.empty()) {
                            syntax->addError(scanner->getLine(), "Invalid expression: No LVAL");
                            return false;
                        }
                        AstExpression *lval = checkExpression(output.top(), varType);
                        output.pop();
                        
                        AstBinaryOp *op = static_cast<AstBinaryOp *>(opStack.top());
                        opStack.pop();
                        
                        op->setLVal(lval);
                        op->setRVal(rval);
                        output.push(op);
                    }
                }
                
                if (token.type == Plus) {
                    AstAddOp *add = new AstAddOp;
                    opStack.push(add);
                } else if (token.type == And) {
                    opStack.push(new AstAndOp);
                } else if (token.type == Or) {
                    opStack.push(new AstOrOp);
                } else if (token.type == Xor) {
                    opStack.push(new AstXorOp);
                } else {
                    if (lastWasOp) {
                        opStack.push(new AstNegOp);
                    } else {
                        AstSubOp *sub = new AstSubOp;
                        opStack.push(sub);
                    }
                }
                
                lastWasOp = true;
            } break;
            
            case Mul: {
                lastWasOp = true;
                AstMulOp *mul = new AstMulOp;
                opStack.push(mul);
            } break;
            
            case Div: {
                lastWasOp = true;
                AstDivOp *div = new AstDivOp;
                opStack.push(div);
            } break;
            
            case EQ: opStack.push(new AstEQOp); lastWasOp = true; break;
            case NEQ: opStack.push(new AstNEQOp); lastWasOp = true; break;
            case GT: opStack.push(new AstGTOp); lastWasOp = true; break;
            case LT: opStack.push(new AstLTOp); lastWasOp = true; break;
            case GTE: opStack.push(new AstGTEOp); lastWasOp = true; break;
            case LTE: opStack.push(new AstLTEOp); lastWasOp = true; break;
            
            /*case EQ:
            case NEQ:
            case GT:
            case LT:
            case GTE:
            case LTE: {
                if (opStack.size() > 0) {
                    AstType type = opStack.top()->getType();
                    if (type == AstType::LogicalAnd || type == AstType::LogicalOr) {
                        if (output.empty()) {
                            syntax->addError(scanner->getLine(), "Invalid expression: No RVAL");
                            return false;
                        }
                        AstExpression *rval = checkExpression(output.top(), varType);
                        output.pop();
                        
                        if (output.empty()) {
                            syntax->addError(scanner->getLine(), "Invalid expression: No LVAL");
                            return false;
                        }
                        AstExpression *lval = checkExpression(output.top(), varType);
                        output.pop();
                        
                        AstBinaryOp *op = static_cast<AstBinaryOp *>(opStack.top());
                        opStack.pop();
                        
                        op->setLVal(lval);
                        op->setRVal(rval);
                        output.push(op);
                    }
                }
                
                switch (token.type) {
                    case EQ: opStack.push(new AstEQOp); break;
                    case NEQ: opStack.push(new AstNEQOp); break;
                    case GT: opStack.push(new AstGTOp); break;
                    case LT: opStack.push(new AstLTOp); break;
                    case GTE: opStack.push(new AstGTEOp); break;
                    case LTE: opStack.push(new AstLTEOp); break;
                    default: {}
                }
                
                lastWasOp = true;
            } break;*/
            
            /*case Logical_And: {
                lastWasOp = true;
                opStack.push(new AstLogicalAndOp);
            } break;
            
            case Logical_Or: {
                lastWasOp = true;
                opStack.push(new AstLogicalOrOp);
            } break;*/
            
            case Logical_And:
            case Logical_Or: {
                if (opStack.size() > 0) {
                    AstType type = opStack.top()->getType();
                    switch (type) {
                        case AstType::EQ:
                        case AstType::NEQ:
                        case AstType::GT:
                        case AstType::LT:
                        case AstType::GTE:
                        case AstType::LTE: {
                            if (output.empty()) {
                                syntax->addError(scanner->getLine(), "Invalid expression: No RVAL");
                                return false;
                            }
                            AstExpression *rval = checkExpression(output.top(), varType);
                            output.pop();
                            
                            if (output.empty()) {
                                syntax->addError(scanner->getLine(), "Invalid expression: No LVAL");
                                return false;
                            }
                            AstExpression *lval = checkExpression(output.top(), varType);
                            output.pop();
                            
                            AstBinaryOp *op = static_cast<AstBinaryOp *>(opStack.top());
                            opStack.pop();
                            
                            op->setLVal(lval);
                            op->setRVal(rval);
                            output.push(op);
                        } break;
                        
                        default: {}
                    }
                }
                
                if (token.type == Logical_And) opStack.push(new AstLogicalAndOp);
                else if (token.type == Logical_Or) opStack.push(new AstLogicalOrOp);
                
                lastWasOp = true;
            } break;
            
            case Comma: break;
            
            default: {
                syntax->addError(scanner->getLine(), "Invalid token in expression.");
                return false;
            }
        }
        
        if (!lastWasOp && opStack.size() > 0) {
            if (opStack.top()->getType() == AstType::Neg) {
                AstExpression *val = checkExpression(output.top(), varType);
                output.pop();
                
                AstNegOp *op = static_cast<AstNegOp *>(opStack.top());
                opStack.pop();
                op->setVal(val);
                output.push(op);
            }
        }
        
        token = scanner->getNext();
    }
    
    if (token.type == Eof) {
        syntax->addError(scanner->getLine(), "Invalid expression-> missing \';\'.");
        return false;
    }
    
    // Build the expression
    while (opStack.size() > 0) {
        if (output.empty()) {
            syntax->addError(scanner->getLine(), "Invalid expression: No RVAL");
            return false;
        }
        AstExpression *rval = checkExpression(output.top(), varType);
        output.pop();
        
        if (output.empty()) {
            syntax->addError(scanner->getLine(), "Invalid expression: No LVAL");
            return false;
        }
        AstExpression *lval = checkExpression(output.top(), varType);
        output.pop();
        
        AstBinaryOp *op = static_cast<AstBinaryOp *>(opStack.top());
        opStack.pop();
        
        op->setLVal(lval);
        op->setRVal(rval);
        output.push(op);
    }
    
    // Add the expressions back
    if (output.size() == 0) {
        return true;
    }
    
    // Type check the top
    AstExpression *expr = checkExpression(output.top(), varType);
    
    if (stmt == nullptr) {
        if ((*dest) == nullptr) {
            *dest = expr;
        } else if ((*dest)->getType() == AstType::FuncCallExpr) {
            AstFuncCallExpr *fc = static_cast<AstFuncCallExpr *>(*dest);
            fc->addArgument(expr);
        } else {
            *dest = expr;
        }
    } else {
        stmt->addExpression(expr);
    }
    
    return true;
}

// This is meant mainly for literals; it checks to make sure all the types in
// the expression agree in type. LLVM will have a problem if not
AstExpression *Parser::checkExpression(AstExpression *expr, DataType varType) {
    switch (expr->getType()) {
        case AstType::I32L: {
            // Change to byte literals
            if (varType == DataType::I8 || varType == DataType::U8) {
                AstI32 *i32 = static_cast<AstI32 *>(expr);
                AstI8 *byte = new AstI8(i32->getValue());
                expr = byte;
                
            // Change to word literals
            } else if (varType == DataType::I16 || varType == DataType::U16) {
                AstI32 *i32 = static_cast<AstI32 *>(expr);
                AstI16 *i16 = new AstI16(i32->getValue());
                expr = i16;
                
            // Change to qword literals
            } else if (varType == DataType::I64 || varType == DataType::U64) {
                AstI32 *i32 = static_cast<AstI32 *>(expr);
                AstI64 *i64 = new AstI64(i32->getValue());
                expr = i64;
            }
        } break;
            
        default: {}
    }
    
    return expr;
}

// The debug function for the scanner
void Parser::debugScanner() {
    std::cout << "Debugging scanner..." << std::endl;
    
    Token t;
    do {
        t = scanner->getNext();
        t.print();
    } while (t.type != Eof);
}

// Checks to see if a string is a constant
int Parser::isConstant(std::string name) {
    if (globalConsts.find(name) != globalConsts.end()) {
        return 1;
    }
    
    if (localConsts.find(name) != localConsts.end()) {
        return 2;
    }
    
    return 0;
}

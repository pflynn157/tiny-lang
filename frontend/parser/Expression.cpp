//
// Copyright 2021 Patrick Flynn
// This file is part of the Tiny Lang compiler.
// Tiny Lang is licensed under the BSD-3 license. See the COPYING file for more information.
//
#include <iostream>

#include <parser/Parser.hpp>
#include <ast.hpp>

// Builds a constant expression value
AstExpression *Parser::buildConstExpr(Token token) {
    switch (token.type) {
        case True: return new AstI32(1);
        case False: return new AstI32(0);
        case CharL: return new AstChar(token.i8_val);
        case Int32: return new AstI32(token.i32_val);
        case String: return new AstString(token.id_val);
        
        default: {}
    }
    
    return nullptr;
}

// Applies higher precedence for an operator
bool Parser::applyHigherPred(ExprContext *ctx) {
    if (ctx->output.empty()) {
        syntax->addError(scanner->getLine(), "Invalid expression: No RVAL");
        return false;
    }
    AstExpression *rval = checkExpression(ctx->output.top(), ctx->varType);
    ctx->output.pop();
    
    if (ctx->output.empty()) {
        syntax->addError(scanner->getLine(), "Invalid expression: No LVAL");
        return false;
    }
    AstExpression *lval = checkExpression(ctx->output.top(), ctx->varType);
    ctx->output.pop();
    
    AstBinaryOp *op = static_cast<AstBinaryOp *>(ctx->opStack.top());
    ctx->opStack.pop();
    
    op->setLVal(lval);
    op->setRVal(rval);
    ctx->output.push(op);
    
    return true;
}

// Builds an expression
bool Parser::buildExpression(AstStatement *stmt, DataType currentType, TokenType stopToken, TokenType separateToken,
                             AstExpression **dest, bool isConst) {
    //std::stack<AstExpression *> output;
    //std::stack<AstExpression *> opStack;
    ExprContext *ctx = new ExprContext;
    ctx->varType = currentType;
    int currentLine = scanner->getLine();
    
    DataType varType = currentType;
    
    bool lastWasOp = true;

    Token token = scanner->getNext();
    while (token.type != Eof && token.type != stopToken) {
        if (token.type == separateToken && ctx->output.size() > 0) {
            AstExpression *expr = ctx->output.top();
            ctx->output.pop();
            
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
            case True:
            case False:
            case CharL:
            case Int32:
            case String: {
                lastWasOp = false;
                AstExpression *expr = buildConstExpr(token);
                ctx->output.push(expr);
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
                    ctx->output.push(acc);
                } else if (token.type == LParen) {
                    if (currentLine != scanner->getLine()) {
                        syntax->addWarning(scanner->getLine(), "Function call on newline- possible logic error.");
                    }
                    
                    if (!isFunc(name)) {
                        syntax->addError(scanner->getLine(), "Unknown function call.");
                        return false;
                    }
                
                    AstFuncCallExpr *fc = new AstFuncCallExpr(name);
                    AstExpression *fcExpr = fc;
                    buildExpression(nullptr, varType, RParen, Comma, &fcExpr);
                    
                    ctx->output.push(fc);
                } else if (token.type == Dot) {
                    // TODO: Search for structures here

                    Token idToken = scanner->getNext();
                    if (idToken.type != Id) {
                        syntax->addError(scanner->getLine(), "Expected identifier.");
                        return false;
                    }
                    
                    AstStructAccess *val = new AstStructAccess(name, idToken.id_val);
                    ctx->output.push(val);
                } else {
                    int constVal = isConstant(name);
                    if (constVal > 0) {
                        if (constVal == 1) {
                            AstExpression *expr = globalConsts[name].second;
                            ctx->output.push(expr);
                        } else if (constVal == 2) {
                            AstExpression *expr = localConsts[name].second;
                            ctx->output.push(expr);
                        }
                    } else {
                        if (isVar(name)) {
                            AstID *id = new AstID(name);
                            ctx->output.push(id);
                        } else {
                            syntax->addError(scanner->getLine(), "Unknown variable.");
                            return false;
                        }
                    }
                    
                    scanner->rewind(token);
                }
            } break;
            
            case Plus: 
            case Minus:
            case And:
            case Or:
            case Xor: {
                if (ctx->opStack.size() > 0) {
                    AstType type = ctx->opStack.top()->getType();
                    if (type == AstType::Mul || type == AstType::Div) {
                        if (!applyHigherPred(ctx)) return false;
                    }
                }
                
                if (token.type == Plus) {
                    AstAddOp *add = new AstAddOp;
                    ctx->opStack.push(add);
                } else if (token.type == And) {
                    ctx->opStack.push(new AstAndOp);
                } else if (token.type == Or) {
                    ctx->opStack.push(new AstOrOp);
                } else if (token.type == Xor) {
                    ctx->opStack.push(new AstXorOp);
                } else {
                    if (lastWasOp) {
                        ctx->opStack.push(new AstNegOp);
                    } else {
                        AstSubOp *sub = new AstSubOp;
                        ctx->opStack.push(sub);
                    }
                }
                
                lastWasOp = true;
            } break;
            
            case Mul: {
                lastWasOp = true;
                AstMulOp *mul = new AstMulOp;
                ctx->opStack.push(mul);
            } break;
            
            case Div: {
                lastWasOp = true;
                AstDivOp *div = new AstDivOp;
                ctx->opStack.push(div);
            } break;
            
            case EQ: ctx->opStack.push(new AstEQOp); lastWasOp = true; break;
            case NEQ: ctx->opStack.push(new AstNEQOp); lastWasOp = true; break;
            case GT: ctx->opStack.push(new AstGTOp); lastWasOp = true; break;
            case LT: ctx->opStack.push(new AstLTOp); lastWasOp = true; break;
            case GTE: ctx->opStack.push(new AstGTEOp); lastWasOp = true; break;
            case LTE: ctx->opStack.push(new AstLTEOp); lastWasOp = true; break;
            
            case Logical_And:
            case Logical_Or: {
                if (ctx->opStack.size() > 0) {
                    AstType type = ctx->opStack.top()->getType();
                    switch (type) {
                        case AstType::EQ:
                        case AstType::NEQ:
                        case AstType::GT:
                        case AstType::LT:
                        case AstType::GTE:
                        case AstType::LTE: {
                            if (!applyHigherPred(ctx)) return false;
                        } break;
                        
                        default: {}
                    }
                }
                
                if (token.type == Logical_And) ctx->opStack.push(new AstLogicalAndOp);
                else if (token.type == Logical_Or) ctx->opStack.push(new AstLogicalOrOp);
                
                lastWasOp = true;
            } break;
            
            case Comma: break;
            
            default: {
                syntax->addError(scanner->getLine(), "Invalid token in expression.");
                return false;
            }
        }
        
        if (!lastWasOp && ctx->opStack.size() > 0) {
            if (ctx->opStack.top()->getType() == AstType::Neg) {
                AstExpression *val = checkExpression(ctx->output.top(), varType);
                ctx->output.pop();
                
                AstNegOp *op = static_cast<AstNegOp *>(ctx->opStack.top());
                ctx->opStack.pop();
                op->setVal(val);
                ctx->output.push(op);
            }
        }
        
        token = scanner->getNext();
    }
    
    if (token.type == Eof) {
        syntax->addError(scanner->getLine(), "Invalid expression-> missing \';\'.");
        return false;
    }
    
    // Build the expression
    while (ctx->opStack.size() > 0) {
        if (ctx->output.empty()) {
            syntax->addError(scanner->getLine(), "Invalid expression: No RVAL");
            return false;
        }
        AstExpression *rval = checkExpression(ctx->output.top(), varType);
        ctx->output.pop();
        
        if (ctx->output.empty()) {
            syntax->addError(scanner->getLine(), "Invalid expression: No LVAL");
            return false;
        }
        AstExpression *lval = checkExpression(ctx->output.top(), varType);
        ctx->output.pop();
        
        AstBinaryOp *op = static_cast<AstBinaryOp *>(ctx->opStack.top());
        ctx->opStack.pop();
        
        op->setLVal(lval);
        op->setRVal(rval);
        ctx->output.push(op);
    }
    
    // Add the expressions back
    if (ctx->output.size() == 0) {
        return true;
    }
    
    // Type check the top
    AstExpression *expr = checkExpression(ctx->output.top(), varType);
    
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

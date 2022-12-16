//
// Copyright 2021-2022 Patrick Flynn
// This file is part of the Tiny Lang compiler.
// Tiny Lang is licensed under the BSD-3 license. See the COPYING file for more information.
//
#include <iostream>
#include <algorithm>

#include <parser/Parser.hpp>
#include <ast/ast_builder.hpp>

Parser::Parser(std::string input) {
    this->input = input;
    scanner = new Scanner(input);
    
    tree = new AstTree(input);
    syntax = new ErrorManager;
    
    // Add the built-in functions
    //string malloc(string)
    funcs.push_back("malloc");
    AstExternFunction *FT1 = new AstExternFunction("malloc");
    FT1->addArgument(Var(AstBuilder::buildInt32Type(), "size"));
    FT1->setDataType(AstBuilder::buildStringType());
    tree->addGlobalStatement(FT1);
    
    //println(string)
    funcs.push_back("println");
    AstExternFunction *FT2 = new AstExternFunction("println");
    FT2->setVarArgs();
    FT2->addArgument(Var(AstBuilder::buildStringType(), "str"));
    FT2->setDataType(AstBuilder::buildVoidType());
    tree->addGlobalStatement(FT2);
    
    //print(string)
    funcs.push_back("print");
    AstExternFunction *FT3 = new AstExternFunction("print");
    FT3->setVarArgs();
    FT3->addArgument(Var(AstBuilder::buildStringType(), "str"));
    FT3->setDataType(AstBuilder::buildVoidType());
    tree->addGlobalStatement(FT3);
    
    //i32 strlen(string)
    funcs.push_back("strlen");
    AstExternFunction *FT4 = new AstExternFunction("strlen");
    FT4->addArgument(Var(AstBuilder::buildStringType(), "str"));
    FT4->setDataType(AstBuilder::buildInt32Type());
    tree->addGlobalStatement(FT4);
    
    //i32 stringcmp(string, string)
    funcs.push_back("stringcmp");
    AstExternFunction *FT5 = new AstExternFunction("stringcmp");
    FT5->addArgument(Var(AstBuilder::buildStringType(), "str"));
    FT5->addArgument(Var(AstBuilder::buildStringType(), "str"));
    FT5->setDataType(AstBuilder::buildInt32Type());
    tree->addGlobalStatement(FT5);
    
    //string strcat_str(string, string)
    funcs.push_back("strcat_str");
    AstExternFunction *FT6 = new AstExternFunction("strcat_str");
    FT6->addArgument(Var(AstBuilder::buildStringType(), "str"));
    FT6->addArgument(Var(AstBuilder::buildStringType(), "str"));
    FT6->setDataType(AstBuilder::buildStringType());
    tree->addGlobalStatement(FT6);
    
    //string strcat_char(string, char)
    funcs.push_back("strcat_char");
    AstExternFunction *FT7 = new AstExternFunction("strcat_char");
    FT7->addArgument(Var(AstBuilder::buildStringType(), "str"));
    FT7->addArgument(Var(AstBuilder::buildCharType(), "c"));
    FT7->setDataType(AstBuilder::buildStringType());
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
            
            case Eof: break;
            
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
bool Parser::buildBlock(AstBlock *block, AstNode *parent) {
    Token token = scanner->getNext();
    while (token.type != End && token.type != Eof) {
        bool code = true;
        bool end = false;
        
        switch (token.type) {
            case VarD: code = buildVariableDec(block); break;
            case Struct: code = buildStructDec(block); break;
            case Const: code = buildConst(false); break;
            
            case Id: {
                Token idToken = token;
                token = scanner->getNext();
                
                if (token.type == Assign || token.type == LBracket || token.type == Dot) {
                    scanner->rewind(token);
                    scanner->rewind(idToken);
                    code = buildVariableAssign(block, idToken);
                } else if (token.type == LParen) {
                    code = buildFunctionCallStmt(block, idToken);
                } else {
                    syntax->addError(scanner->getLine(), "Invalid use of identifier.");
                    token.print();
                    return false;
                }
            } break;
            
            case Return: code = buildReturn(block); break;
            
            // Handle conditionals
            case If: code = buildConditional(block); break;
            case Elif: {
                AstIfStmt *condParent = static_cast<AstIfStmt *>(parent);
                code = buildConditional(condParent->getFalseBlock());
                end = true;
            } break;
            case Else: {
                AstIfStmt *condParent = static_cast<AstIfStmt *>(parent);
                buildBlock(condParent->getFalseBlock());
                end = true;
            } break;
            
            // Handle loops
            case While: code = buildWhile(block); break;
            case Break: code = buildLoopCtrl(block, true); break;
            case Continue: code = buildLoopCtrl(block, false); break;
            
            default: {
                syntax->addError(scanner->getLine(), "Invalid token in block.");
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

// This is meant mainly for literals; it checks to make sure all the types in
// the expression agree in type. LLVM will have a problem if not
AstExpression *Parser::checkExpression(AstExpression *expr, AstDataType *varType) {
    if (!varType) return expr;

    switch (expr->getType()) {
        case V_AstType::I32L: {
            // Change to byte literals
            if (varType->getType() == V_AstType::Int8) {
                AstI32 *i32 = static_cast<AstI32 *>(expr);
                AstI8 *byte = new AstI8(i32->getValue());
                expr = byte;
                
            // Change to word literals
            } else if (varType->getType() == V_AstType::Int16) {
                AstI32 *i32 = static_cast<AstI32 *>(expr);
                AstI16 *i16 = new AstI16(i32->getValue());
                expr = i16;
                
            // Change to qword literals
            } else if (varType->getType() == V_AstType::Int64) {
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

bool Parser::isFunc(std::string name) {
    if (std::find(funcs.begin(), funcs.end(), name) != funcs.end()) {
        return true;
    }
    return false;
}

//
// Builds a data type from the token stream
//
AstDataType *Parser::buildDataType(bool checkBrackets) {
    Token token = scanner->getNext();
    AstDataType *dataType = nullptr;
    
    switch (token.type) {
        case Bool: dataType = AstBuilder::buildBoolType(); break;
        case Char: dataType = AstBuilder::buildCharType(); break;
        case I8: dataType = AstBuilder::buildInt8Type(); break;
        case U8: dataType = AstBuilder::buildInt8Type(true); break;
        case I16: dataType = AstBuilder::buildInt16Type(); break;
        case U16: dataType = AstBuilder::buildInt16Type(true); break;
        case I32: dataType = AstBuilder::buildInt32Type(); break;
        case U32: dataType = AstBuilder::buildInt32Type(true); break;
        case I64: dataType = AstBuilder::buildInt64Type(); break;
        case U64: dataType = AstBuilder::buildInt64Type(true); break;
        case Str: dataType = AstBuilder::buildStringType(); break;
        
        case Id: {
            bool isStruct = false;
            for (auto s : tree->getStructs()) {
                if (s->getName() == token.id_val) {
                    isStruct = true;
                    break;
                }
            }
                
            if (isStruct) {
                dataType = AstBuilder::buildStructType(token.id_val);
            }
        } break;
        
        default: {}
    }

    if (checkBrackets) {
        token = scanner->getNext();
        if (token.type == LBracket) {
            token = scanner->getNext();
            if (token.type != RBracket) {
                syntax->addError(scanner->getLine(), "Invalid pointer type.");
                return nullptr;
            }
            
            dataType = AstBuilder::buildPointerType(dataType);
        } else {
            scanner->rewind(token);
        }
    }
    
    return dataType;
}


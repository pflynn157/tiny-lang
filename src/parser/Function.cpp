//
// Copyright 2021-2022 Patrick Flynn
// This file is part of the Tiny Lang compiler.
// Tiny Lang is licensed under the BSD-3 license. See the COPYING file for more information.
//
#include <iostream>

#include <parser/Parser.hpp>
#include <ast/ast.hpp>
#include <ast/ast_builder.hpp>

// Returns the function arguments
bool Parser::getFunctionArgs(std::vector<Var> &args) {
    Token token = scanner->getNext();
    if (token.type == LParen) {
        token = scanner->getNext();
        while (token.type != Eof && token.type != RParen) {
            Token t1 = token;
            Token t2 = scanner->getNext();
            Var v;
            
            if (t1.type != Id) {
                syntax->addError(scanner->getLine(), "Invalid function argument: Expected name.");
                return false;
            }
            
            if (t2.type != Colon) {
                syntax->addError(scanner->getLine(), "Invalid function argument: Expected \':\'.");
                return false;
            }
            
            v.type = buildDataType();
            v.name = t1.id_val;
            vars.push_back(t1.id_val);
            
            token = scanner->getNext();
            if (token.type == Comma) {
                token = scanner->getNext();
            }
            
            args.push_back(v);
            typeMap[v.name] = v.type;
        }
    } else {
        scanner->rewind(token);
    }
    
    return true;
}

// Builds a function
bool Parser::buildFunction(Token startToken, std::string className) {
    typeMap.clear();
    localConsts.clear();
    vars.clear();
    
    Token token;
    bool isExtern = false;

    // Handle extern function
    if (startToken.type == Extern) {
        isExtern = true;
    }

    // Make sure we have a function name
    token = scanner->getNext();
    std::string funcName = token.id_val;
    
    if (token.type != Id) {
        syntax->addError(scanner->getLine(), "Expected function name.");
        return false;
    }
    
    // Get arguments
    std::vector<Var> args;
    if (!getFunctionArgs(args)) return false;

    // Check to see if there's any return type
    //std::string retName = "";       // TODO: Do we need this?
    token = scanner->getNext();
    AstDataType *dataType;
    if (token.type == Arrow) {
        dataType = buildDataType();
        token = scanner->getNext();
    }
    else dataType = AstBuilder::buildVoidType();
    
    // Do syntax error check
    if (token.type == SemiColon && !isExtern) {
        syntax->addError(scanner->getLine(), "Expected \';\' for extern function.");
        return false;
    } else if (token.type == Is && isExtern) {
        syntax->addError(scanner->getLine(), "Expected \'is\' keyword.");
        return false;
    }

    // Create the function object
    funcs.push_back(funcName);
    
    if (isExtern) {
        AstExternFunction *ex = new AstExternFunction(funcName);
        ex->setArguments(args);
        ex->setDataType(dataType);
        tree->addGlobalStatement(ex);
        return true;
    }
    
    AstFunction *func = new AstFunction(funcName);
    func->setDataType(dataType);
    func->setArguments(args);
    tree->addGlobalStatement(func);
    
    // Build the body
    int stopLayer = 0;
    if (!buildBlock(func->getBlock())) return false;
    
    // Make sure we end with a return statement
    V_AstType lastType = func->getBlock()->getBlock().back()->getType();
    if (lastType == V_AstType::Return) {
        AstStatement *ret = func->getBlock()->getBlock().back();
        if (func->getDataType()->getType() == V_AstType::Void && ret->hasExpression()) {
            syntax->addError(scanner->getLine(), "Cannot return from void function.");
            return false;
        } else if (!ret->hasExpression()) {
            syntax->addError(scanner->getLine(), "Expected return value.");
            return false;
        }
    } else {
        if (func->getDataType()->getType() == V_AstType::Void) {
            func->addStatement(new AstReturnStmt);
        } else {
            syntax->addError(scanner->getLine(), "Expected return statement.");
            return false;
        }
    }
    
    return true;
}

// Builds a function call
bool Parser::buildFunctionCallStmt(AstBlock *block, Token idToken) {
    // Make sure the function exists
    if (!isFunc(idToken.id_val)) {
        syntax->addError(scanner->getLine(), "Unknown function.");
        return false;
    }

    AstFuncCallStmt *fc = new AstFuncCallStmt(idToken.id_val);
    block->addStatement(fc);
    
    AstExpression *args = buildExpression(nullptr, SemiColon, false, true);
    if (!args) return false;
    fc->setExpression(args);
    
    return true;
}

// Builds a return statement
bool Parser::buildReturn(AstBlock *block) {
    AstReturnStmt *stmt = new AstReturnStmt;
    block->addStatement(stmt);
    
    AstExpression *arg = buildExpression(nullptr);
    if (!arg) return false;
    stmt->setExpression(arg);
    
    return true;
}


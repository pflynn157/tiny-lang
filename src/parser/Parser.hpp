//
// Copyright 2021-2022 Patrick Flynn
// This file is part of the Tiny Lang compiler.
// Tiny Lang is licensed under the BSD-3 license. See the COPYING file for more information.
//
#pragma once

#include <string>
#include <map>

#include <lex.hpp>
#include <parser/ErrorManager.hpp>
#include <ast/ast.hpp>

// The parser class
// The parser is in charge of performing all parsing and AST-building tasks
// It is also in charge of the error manager

class Parser {
public:
    explicit Parser(std::string input);
    ~Parser();
    
    bool parse();
    
    AstTree *getTree() { return tree; }
    
    void debugScanner();
protected:
    // Function.cpp
    bool getFunctionArgs(AstBlock *block, std::vector<Var> &args);
    bool buildFunction(Token startToken, std::string className = "");
    bool buildFunctionCallStmt(AstBlock *block, Token idToken);
    bool buildReturn(AstBlock *block);
    
    // Variable.cpp
    bool buildVariableDec(AstBlock *block);
    bool buildVariableAssign(AstBlock *block, Token idToken);
    bool buildConst(bool isGlobal);
    
    // Flow.cpp
    bool buildConditional(AstBlock *block);
    bool buildWhile(AstBlock *block);
    bool buildLoopCtrl(AstBlock *block, bool isBreak);
    
    // Structure.cpp
    bool buildStruct();
    bool buildStructMember(AstStruct *str, Token token);
    bool buildStructDec(AstBlock *block);
    
    // Expression.cpp
    struct ExprContext {
        std::stack<AstExpression *> output;
        std::stack<AstOp *> opStack;
        AstDataType *varType;
        bool lastWasOp = true;
    };
    
    AstExpression *buildConstExpr(Token token);
    bool buildOperator(Token token, ExprContext *ctx);
    bool buildIDExpr(AstBlock *block, Token token, ExprContext *ctx);
    bool applyHigherPred(ExprContext *ctx);
    bool applyAssoc(ExprContext *ctx);
    AstExpression *buildExpression(AstBlock *block, AstDataType *currentType, TokenType stopToken = SemiColon, bool isConst = false, bool buildList = false);
    AstExpression *checkExpression(AstExpression *expr, AstDataType *varType);
    
    bool buildBlock(AstBlock *block, AstNode *parent = nullptr);
    AstExpression *checkCondExpression(AstBlock *block, AstExpression *toCheck);
    int isConstant(std::string name);
    bool isVar(std::string name);
    bool isFunc(std::string name);
    AstDataType *buildDataType(bool checkBrackets = true);
private:
    std::string input = "";
    Scanner *scanner;
    AstTree *tree;
    ErrorManager *syntax;
    
    std::map<std::string, std::pair<AstDataType *, AstExpression*>> globalConsts;
    std::map<std::string, std::pair<AstDataType *, AstExpression*>> localConsts;
    std::vector<std::string> funcs;
};


//
// Copyright 2021 Patrick Flynn
// This file is part of the Orka compiler.
// Orka is licensed under the BSD-3 license. See the COPYING file for more information.
//
#include <parser/Parser.hpp>
#include <ast.hpp>

// Called if a conditional statement has only one operand. If it does,
// we have to expand to have two operands before we get down to the
// compiler layer
AstExpression *Parser::checkCondExpression(AstExpression *toCheck) {
    AstExpression *expr = toCheck;
    
    switch (toCheck->getType()) {
        case AstType::ID: {
            AstID *id = static_cast<AstID *>(toCheck);
            DataType dataType = typeMap[id->getValue()].first;
            
            AstEQOp *eq = new AstEQOp;
            eq->setLVal(id);
            
            switch (dataType) {
                case DataType::Bool: eq->setRVal(new AstI32(1)); break;
                case DataType::I8:
                case DataType::U8: eq->setRVal(new AstI8(1)); break;
                case DataType::I16:
                case DataType::U16: eq->setRVal(new AstI16(1)); break;
                case DataType::I32:
                case DataType::U32: eq->setRVal(new AstI32(1)); break;
                case DataType::I64:
                case DataType::U64: eq->setRVal(new AstI64(1)); break;
                
                default: {}
            }
            
            expr = eq;
        } break;
        
        default: {}
    }
    
    return expr;
}

// Builds a conditional statement
bool Parser::buildConditional(AstBlock *block) {
    AstIfStmt *cond = new AstIfStmt;
    if (!buildExpression(cond, DataType::Void, Then)) return false;
    block->addStatement(cond);
    
    AstExpression *expr = checkCondExpression(cond->getExpressions().at(0));
    cond->clearExpressions();
    cond->addExpression(expr);

    ++layer;
    buildBlock(cond->getBlockStmt(), layer, cond);
    
    return true;
}

// Builds an ELIF statement
bool Parser::buildElif(AstIfStmt *block) {
    AstElifStmt *elif = new AstElifStmt;
    if (!buildExpression(elif, DataType::Void, Then)) return false;
    block->addBranch(elif);
    
    AstExpression *expr = checkCondExpression(elif->getExpressions().at(0));
    elif->clearExpressions();
    elif->addExpression(expr);
    
    buildBlock(elif->getBlockStmt(), layer, block, true);
    return true;
}

// Builds an ELSE statement
bool Parser::buildElse(AstIfStmt *block) {
    AstElseStmt *elsee = new AstElseStmt;
    block->addBranch(elsee);
    
    buildBlock(elsee->getBlockStmt(), layer);
    return true;
}

// Builds a while statement
bool Parser::buildWhile(AstBlock *block) {
    AstWhileStmt *loop = new AstWhileStmt;
    if (!buildExpression(loop, DataType::Void, Do)) return false;
    block->addStatement(loop);
    
    AstExpression *expr = checkCondExpression(loop->getExpressions().at(0));
    loop->clearExpressions();
    loop->addExpression(expr);
    
    ++layer;
    buildBlock(loop->getBlockStmt(), layer);
    
    return true;
}

// Builds a loop keyword
bool Parser::buildLoopCtrl(AstBlock *block, bool isBreak) {
    if (isBreak) block->addStatement(new AstBreak);
    else block->addStatement(new AstContinue);
    
    Token token = scanner->getNext();
    if (token.type != SemiColon) {
        syntax->addError(scanner->getLine(), "Expected \';\' after break or continue.");
        return false;
    }
    
    return true;
}


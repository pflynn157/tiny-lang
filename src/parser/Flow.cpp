//
// Copyright 2021-2022 Patrick Flynn
// This file is part of the Tiny Lang compiler.
// Tiny Lang is licensed under the BSD-3 license. See the COPYING file for more information.
//
#include <parser/Parser.hpp>
#include <ast/ast.hpp>

// Called if a conditional statement has only one operand. If it does,
// we have to expand to have two operands before we get down to the
// compiler layer
AstExpression *Parser::checkCondExpression(AstBlock *block, AstExpression *toCheck) {
    AstExpression *expr = toCheck;
    
    switch (toCheck->getType()) {
        case V_AstType::ID: {
            AstID *id = static_cast<AstID *>(toCheck);
            AstDataType *dataType = block->getDataType(id->getValue());            
            AstEQOp *eq = new AstEQOp;
            eq->setLVal(id);
            
            switch (dataType->getType()) {
                case V_AstType::Bool: eq->setRVal(new AstI32(1)); break;
                case V_AstType::Int8: eq->setRVal(new AstI8(1)); break;
                case V_AstType::Int16: eq->setRVal(new AstI16(1)); break;
                case V_AstType::Int32: eq->setRVal(new AstI32(1)); break;
                case V_AstType::Int64: eq->setRVal(new AstI64(1)); break;
                
                default: {}
            }
            
            expr = eq;
        } break;
        
        case V_AstType::I32L: {
            AstEQOp *eq = new AstEQOp;
            eq->setLVal(expr);
            eq->setRVal(new AstI32(1));
            expr = eq;
        } break;
        
        default: {}
    }
    
    return expr;
}

// Builds a conditional statement
bool Parser::buildConditional(AstBlock *block) {
    AstIfStmt *cond = new AstIfStmt;
    AstExpression *arg = buildExpression(block, nullptr, Then);
    if (!arg) return false;
    cond->setExpression(arg);
    block->addStatement(cond);
    
    AstExpression *expr = checkCondExpression(block, cond->getExpression());
    cond->setExpression(expr);
    
    AstBlock *trueBlock = new AstBlock;
    trueBlock->mergeSymbols(block);
    cond->setTrueBlock(trueBlock);
    
    AstBlock *falseBlock = new AstBlock;
    falseBlock->mergeSymbols(block);
    cond->setFalseBlock(falseBlock);
    buildBlock(trueBlock, cond);
    
    return true;
}

// Builds a while statement
bool Parser::buildWhile(AstBlock *block) {
    AstWhileStmt *loop = new AstWhileStmt;
    AstExpression *arg = buildExpression(block, nullptr, Do);
    if (!arg) return false;
    loop->setExpression(arg);
    block->addStatement(loop);
    
    AstExpression *expr = checkCondExpression(block, loop->getExpression());
    loop->setExpression(expr);
    
    AstBlock *block2 = new AstBlock;
    block2->mergeSymbols(block);
    buildBlock(block2);
    loop->setBlock(block2);
    
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


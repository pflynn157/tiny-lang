//
// Copyright 2021 Patrick Flynn
// This file is part of the Tiny Lang compiler.
// Tiny Lang is licensed under the BSD-3 license. See the COPYING file for more information.
//
#include <LLIR/Compiler.hpp>

// Translates an AST IF statement to LLVM
void Compiler::compileIfStatement(AstStatement *stmt) {
    AstIfStmt *condStmt = static_cast<AstIfStmt *>(stmt);
    bool hasBranches = condStmt->getBranches().size();

    LLIR::Block *trueBlock = new LLIR::Block("true" + std::to_string(blockCount));
    LLIR::Block *falseBlock = nullptr;
    LLIR::Block *endBlock = new LLIR::Block("end" + std::to_string(blockCount));
    if (hasBranches) falseBlock = new LLIR::Block("false" + std::to_string(blockCount));
    ++blockCount;

    LLIR::Operand *cond = compileValue(stmt->getExpressions().at(0), DataType::Void, trueBlock);
    if (hasBranches) builder->createBr(falseBlock);
    else builder->createBr(endBlock);

    // Align the blocks
    builder->addBlock(trueBlock);

    if (hasBranches) {
        builder->addBlock(falseBlock);
    } else {
    }
    builder->addBlock(endBlock);

    builder->setInsertPoint(trueBlock);
    bool hasBreak = false;
    bool hasEndingRet = false;
    for (auto stmt : condStmt->getBlock()) {
        compileStatement(stmt);
        if (stmt->getType() == AstType::Break) hasBreak = true;
    }
    if (condStmt->getBlock().back()->getType() == AstType::Return) hasEndingRet = true;
    if (!hasBreak && !hasEndingRet) builder->createBr(endBlock);

    // Branches
    bool hadElif = false;
    bool hadElse = false;
    int subCount = 0;

    for (auto stmt : condStmt->getBranches()) {
        if (stmt->getType() == AstType::Elif) {
            AstElifStmt *elifStmt = static_cast<AstElifStmt *>(stmt);
            
            LLIR::Block *trueBlock2 = new LLIR::Block(std::to_string(subCount) + "true" + std::to_string(blockCount));
            LLIR::Block *falseBlock2 = new LLIR::Block(std::to_string(subCount) + "false" + std::to_string(blockCount));
            ++subCount;
            
            // Align
            if (!hadElif) builder->setInsertPoint(falseBlock);
            LLIR::Block *current = builder->getInsertPoint();
            builder->addBlockAfter(current, trueBlock2);
            builder->addBlockAfter(trueBlock2, falseBlock2);
            
            LLIR::Operand *cond = compileValue(stmt->getExpressions().at(0), DataType::Void, trueBlock2);
            builder->createBr(falseBlock2);
            
            builder->setInsertPoint(trueBlock2);
            bool hasBreak = false;
            bool hasEndingRet = false;
            for (auto stmt2 : elifStmt->getBlock()) {
                compileStatement(stmt2);
            }
            if (elifStmt->getBlock().back()->getType() == AstType::Return) hasEndingRet = true;
            if (!hasBreak && !hasEndingRet) builder->createBr(endBlock);
            
            builder->setInsertPoint(falseBlock2);
            hadElif = true;
        } else if (stmt->getType() == AstType::Else) {
            AstElseStmt *elseStmt = static_cast<AstElseStmt *>(stmt);
            
            if (!hadElif) builder->setInsertPoint(falseBlock);
            
            bool hasBreak = false;
            bool hasEndingRet = false;
            for (auto stmt2 : elseStmt->getBlock()) {
                compileStatement(stmt2);
            }
            if (elseStmt->getBlock().back()->getType() == AstType::Return) hasEndingRet = true;
            if (!hasBreak && !hasEndingRet) builder->createBr(endBlock);
            
            hadElse = true;
        }
    }

    if (hadElif && !hadElse) {
        builder->createBr(endBlock);
    }

    // Start at the end block
    builder->setInsertPoint(endBlock);
}

// Translates a while statement to LLVM
void Compiler::compileWhileStatement(AstStatement *stmt) {
    AstWhileStmt *loop = static_cast<AstWhileStmt *>(stmt);

    /*BasicBlock *loopBlock = BasicBlock::Create(*context, "loop_body" + std::to_string(blockCount), currentFunc);
    BasicBlock *loopCmp = BasicBlock::Create(*context, "loop_cmp" + std::to_string(blockCount), currentFunc);
    BasicBlock *loopEnd = BasicBlock::Create(*context, "loop_end" + std::to_string(blockCount), currentFunc);
    ++blockCount;

    BasicBlock *current = builder->GetInsertBlock();
    loopBlock->moveAfter(current);
    loopCmp->moveAfter(loopBlock);
    loopEnd->moveAfter(loopCmp);
    
    breakStack.push(loopEnd);
    continueStack.push(loopCmp);

    builder->CreateBr(loopCmp);
    builder->SetInsertPoint(loopCmp);
    Value *cond = compileValue(stmt->getExpressions().at(0));
    builder->CreateCondBr(cond, loopBlock, loopEnd);

    builder->SetInsertPoint(loopBlock);
    for (auto stmt : loop->getBlock()) {
        compileStatement(stmt);
    }
    builder->CreateBr(loopCmp);
    
    builder->SetInsertPoint(loopEnd);
    
    breakStack.pop();
    continueStack.pop();*/
}

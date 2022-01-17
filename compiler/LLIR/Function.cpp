//
// Copyright 2021 Patrick Flynn
// This file is part of the Tiny Lang compiler.
// Tiny Lang is licensed under the BSD-3 license. See the COPYING file for more information.
//
#include <iostream>

#include <LLIR/Compiler.hpp>
#include <llir.hpp>

//
// Compiles a function and its body
//
void Compiler::compileFunction(AstGlobalStatement *global) {
    symtable.clear();
    typeTable.clear();
    ptrTable.clear();
    structVarTable.clear();
    structArgs.clear();
    
    AstFunction *astFunc = static_cast<AstFunction *>(global);

    std::vector<Var> astVarArgs = astFunc->getArguments();
    LLIR::Type *funcType = translateType(astFunc->getDataType(), astFunc->getPtrType(), astFunc->getDataTypeName());
    currentFuncType = astFunc->getDataType();
    if (currentFuncType == DataType::Struct)
        funcTypeStruct = astFunc->getDataTypeName();
    
    std::vector<LLIR::Type *> args;
    if (astVarArgs.size() > 0) {
        for (auto var : astVarArgs) {
            LLIR::Type *type = translateType(var.type, var.subType, var.typeName);
            if (var.type == DataType::Struct) {
                type = translateType(DataType::Ptr, var.type, var.typeName);
            }
            args.push_back(type);
        }
    }
    
    LLIR::Function *func = LLIR::Function::Create(astFunc->getName(), LLIR::Linkage::Global, funcType);
    func->setArgs(args);
    currentFunc = func;
    mod->addFunction(func);
    
    builder->setCurrentFunction(func);
    builder->createBlock("entry");
    
    // Load and store any arguments
    if (astVarArgs.size() > 0) {
        for (int i = 0; i<astVarArgs.size(); i++) {
            Var var = astVarArgs.at(i);
            
            // Build the alloca for the local var
            //LLIR::Type *type = translateType(var.type, var.subType, var.typeName);
            LLIR::Type *type = args.at(i);
            // TODO: Combine this with below
            if (var.type == DataType::Struct) {
                LLIR::Reg *alloca = builder->createAlloca(type);
               // symtable[var.name] = func->getArg(i);
                symtable[var.name] = alloca;
                typeTable[var.name] = DataType::Ptr;
                ptrTable[var.name] = var.type;
                structVarTable[var.name] = var.typeName;
                structArgs.push_back(var.name);
                
                LLIR::Operand *param = func->getArg(i);
                builder->createStore(type, param, alloca);
                continue;
            }
            
            LLIR::Reg *alloca = builder->createAlloca(type);
            symtable[var.name] = alloca;
            typeTable[var.name] = var.type;
            ptrTable[var.name] = var.subType;
            
            // Store the variable
            LLIR::Operand *param = func->getArg(i);
            builder->createStore(type, param, alloca);
        }
    }

    for (auto stmt : astFunc->getBlock()->getBlock()) {
        compileStatement(stmt);
    }
}

//
// Compiles an extern function declaration
//
void Compiler::compileExternFunction(AstGlobalStatement *global) {
    AstExternFunction *astFunc = static_cast<AstExternFunction *>(global);
    
    std::vector<Var> astVarArgs = astFunc->getArguments();
    /*FunctionType *FT;
    
    Type *retType = translateType(astFunc->getDataType());
    
    if (astVarArgs.size() == 0) {
        FT = FunctionType::get(retType, astFunc->isVarArgs());
    } else {
        std::vector<Type *> args;
        for (auto var : astVarArgs) {
            Type *type = translateType(var.type, var.subType);
            args.push_back(type);
        }
        
        FT = FunctionType::get(retType, args, astFunc->isVarArgs());
    }
    
    Function::Create(FT, Function::ExternalLinkage, astFunc->getName(), mod.get());*/
    LLIR::Type *funcType = translateType(astFunc->getDataType());
    
    std::vector<LLIR::Type *> args;
    if (astVarArgs.size() > 0) {
        for (auto var : astVarArgs) {
            LLIR::Type *type = translateType(var.type, var.subType, var.typeName);
            //if (var.type == DataType::Struct) {
            //    type = PointerType::getUnqual(type);
            //}
            args.push_back(type);
        }
    }
    
    LLIR::Function *func = LLIR::Function::Create(astFunc->getName(), LLIR::Linkage::Extern, funcType);
    func->setArgs(args);
    mod->addFunction(func);
}

//
// Compiles a function call statement
// This is different from an expression; this is where its a free-standing statement
//
// // TODO: We should not do error handeling in the compiler. Check for invalid functions in the AST level
//
void Compiler::compileFuncCallStatement(AstStatement *stmt) {
    AstFuncCallStmt *fc = static_cast<AstFuncCallStmt *>(stmt);
    std::vector<LLIR::Operand *> args;
    
    for (auto stmt : stmt->getExpressions()) {
        LLIR::Operand *val = compileValue(stmt);
        args.push_back(val);
    }
    
    builder->createVoidCall(fc->getName(), args);
}

//
// Compiles a return statement
// TODO: We may want to rethink this some
//
void Compiler::compileReturnStatement(AstStatement *stmt) {
    if (stmt->getExpressionCount() == 1) {
        LLIR::Operand *op = compileValue(stmt->getExpressions().at(0), currentFuncType);
        LLIR::Type *type = translateType(currentFuncType);
        if (currentFuncType == DataType::Struct) {
            LLIR::StructType *type = structTable[funcTypeStruct];
           // LLIR::Operand *ld = builder->createLoad(type, op);
            builder->createRet(type, op);
        } else {
            builder->createRet(type, op);
        }
    } else {
        builder->createRetVoid();
    }
}


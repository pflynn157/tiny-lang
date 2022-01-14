//
// Copyright 2021 Patrick Flynn
// This file is part of the Tiny Lang compiler.
// Tiny Lang is licensed under the BSD-3 license. See the COPYING file for more information.
//
#pragma once

#include <string>
#include <map>
#include <stack>

#include <llir.hpp>
#include <irbuilder.hpp>

#include <ast.hpp>

struct CFlags {
    std::string name;
};

class Compiler {
public:
    explicit Compiler(AstTree *tree, CFlags flags);
    void compile();
    void debug();
    void writeAssembly(bool printTransform = false);
    void assemble();
    void link();
protected:
    void compileStatement(AstStatement *stmt);
    LLIR::Operand *compileValue(AstExpression *expr, DataType dataType = DataType::Void);
    LLIR::Type *translateType(DataType dataType, DataType subType = DataType::Void, std::string typeName = "");
    int getStructIndex(std::string name, std::string member);

    // Function.cpp
    void compileFunction(AstGlobalStatement *global);
    void compileExternFunction(AstGlobalStatement *global);
    void compileFuncCallStatement(AstStatement *stmt);
    void compileReturnStatement(AstStatement *stmt);
    
    // Flow.cpp
    void compileIfStatement(AstStatement *stmt);
    void compileWhileStatement(AstStatement *stmt);
private:
    AstTree *tree;
    CFlags cflags;

    // LLIR stuff
    LLIR::Module *mod;
    LLIR::IRBuilder *builder;
    LLIR::Function *currentFunc;
    DataType currentFuncType = DataType::Void;
    //std::string funcTypeStruct = "";
    
    // The user-defined structure table
    //std::map<std::string, StructType*> structTable;
    //std::map<std::string, std::string> structVarTable;
    //std::map<std::string, std::vector<Type *>> structElementTypeTable;
    
    // Symbol table
    std::map<std::string, LLIR::Reg *> symtable;
    std::map<std::string, DataType> typeTable;
    std::map<std::string, DataType> ptrTable;
    
    // Block stack
    int blockCount = 0;
    //std::stack<BasicBlock *> breakStack;
    //std::stack<BasicBlock *> continueStack;
};


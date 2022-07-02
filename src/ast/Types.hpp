//
// Copyright 2021-2022 Patrick Flynn
// This file is part of the Tiny Lang compiler.
// Tiny Lang is licensed under the BSD-3 license. See the COPYING file for more information.
//
#pragma once

#include <string>
#include <map>

enum class AstType {
    EmptyAst,
    ExternFunc,
    Func,
    Return,
    
    ExprStmt,
    
    FuncCallStmt,
    FuncCallExpr,
    
    VarDec,
    StructDec,
    
    If,
    Elif,
    Else,
    While,
    
    Break,
    Continue,
    
    Neg,
    
    Assign,
    Add,
    Sub,
    Mul,
    Div,
    
    And,
    Or,
    Xor,
    
    EQ,
    NEQ,
    GT,
    LT,
    GTE,
    LTE,
    
    LogicalAnd,
    LogicalOr,
    
    CharL,
    I8L,
    I16L,
    I32L,
    I64L,
    StringL,
    ID,
    ArrayAccess,
    StructAccess,
    
    ExprList
};

enum class DataType {
    Void,
    Bool,
    Char,
    I8, U8,
    I16, U16,
    I32, U32,
    I64, U64,
    String,
    Ptr,
    Struct
};

//
// Contains the variants for all AST nodes
//
enum class V_AstType {
    None,
    
    // Data types
    Void,
    Bool,
    Char,
    Int8,
    Int16,
    Int32,
    Int64,
    String,
    Ptr,
    Struct
};

//
// The base of all AST nodes
//
class AstNode {
public:
    explicit AstNode() {}
    explicit AstNode(V_AstType type) {
        this->type = type;
    }
    
    V_AstType getType() { return type; }
    
    virtual void print() {}
protected:
    V_AstType type = V_AstType::None;
};

//
// The base of all AST data types
//
class AstDataType : public AstNode {
public:
    explicit AstDataType(V_AstType type) : AstNode(type) {}
    explicit AstDataType(V_AstType type, bool isUnsigned) : AstNode(type) {
        this->isUnsigned = isUnsigned;
    }
    
    void setUnsigned(bool isUnsigned) { this->isUnsigned = isUnsigned; }
    
    void print() override;
protected:
    bool isUnsigned = false;
};

// Represents a pointer type
class AstPointerType : public AstDataType {
public:
    explicit AstPointerType(AstDataType *baseType) : AstDataType(V_AstType::Ptr) {
        this->baseType = baseType;
    }
    
    AstDataType *getBaseType() { return baseType; }
    
    void print() override;
protected:
    AstDataType *baseType = nullptr;
};

// Represents a structure type
class AstStructType : public AstDataType {
public:
    explicit AstStructType(std::string name) : AstDataType(V_AstType::Struct) {
        this->name = name;
    }
    
    std::string getName() { return name; }
    
    void print() override;
protected:
    std::string name = "";
};

struct Var {
    explicit Var() {}
    explicit Var(DataType type, DataType subType = DataType::Void, std::string name = "") {
        this->type = type;
        this->subType = subType;
        this->name = name;
    }

    std::string name;
    DataType type;
    DataType subType;
    std::string typeName;
};

// Forward declarations
class AstExpression;
class AstStatement;

// Represents a block
class AstBlock {
public:
    void addStatement(AstStatement *stmt) { block.push_back(stmt); }
    void addStatements(std::vector<AstStatement *> block) { this->block = block; }
    std::vector<AstStatement *> getBlock() { return block; }
    
    void print(int indent = 4);
    std::string dot(std::string parent);
private:
    std::vector<AstStatement *> block;
};

// Represents a struct
class AstStruct {
public:
    explicit AstStruct(std::string name) {
        this->name = name;
    }
    
    void addItem(Var var, AstExpression *defaultExpression) {
        items.push_back(var);
        defaultExpressions[var.name] = defaultExpression;
        
        switch (var.type) {
            case DataType::Char:
            case DataType::I8:
            case DataType::U8: size += 1; break;
            case DataType::I16:
            case DataType::U16: size += 2; break;
            case DataType::Bool:
            case DataType::I32:
            case DataType::U32: size += 4; break;
            case DataType::String:
            case DataType::Ptr:
            case DataType::Struct:
            case DataType::I64:
            case DataType::U64: size += 8; break;
            
            default: {}
        }
    }
    
    std::string getName() { return name; }
    std::vector<Var> getItems() { return items; }
    int getSize() { return size; }
    
    AstExpression *getDefaultExpression(std::string name) {
        return defaultExpressions[name];
    }
    
    void print();
    std::string dot(std::string parent);
private:
    std::string name;
    std::vector<Var> items;
    std::map<std::string, AstExpression*> defaultExpressions;
    int size = 0;
};


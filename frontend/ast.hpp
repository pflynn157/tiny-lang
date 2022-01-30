//
// Copyright 2022 Patrick Flynn
// This file is part of the Eos compiler.
// Eos is licensed under the BSD-3 license. See the COPYING file for more information.
//
#pragma once

#include <string>
#include <vector>

#include <ast/Types.hpp>
#include <ast/Global.hpp>
#include <ast/Statement.hpp>
#include <ast/Expression.hpp>

// Forward declarations
//class AstGlobalStatement;
class AstStatement;
class AstExpression;
class AstEnum;
class AstStruct;

// Represents an AST tree
class AstTree {
public:
    explicit AstTree(std::string file) { this-> file = file; }
    ~AstTree() {}
    
    std::vector<AstGlobalStatement *> getGlobalStatements() {
        return global_statements;
    }
    
    std::vector<AstEnum *> getEnums() {
        return enums;
    }
    
    std::vector<AstStruct *> getStructs() {
        return structs;
    }
    
    bool hasStruct(std::string name) {
        for (AstStruct *s : structs) {
            if (s->getName() == name) return true;
        }
        return false;
    }
    
    void addGlobalStatement(AstGlobalStatement *stmt) {
        global_statements.push_back(stmt);
    }
    
    void addStruct(AstEnum *en) {
        enums.push_back(en);
    }
    
    void addStruct(AstStruct *s) {
        structs.push_back(s);
    }
    
    void print();
private:
    std::string file = "";
    std::vector<AstGlobalStatement *> global_statements;
    std::vector<AstEnum *> enums;
    std::vector<AstStruct *> structs;
};

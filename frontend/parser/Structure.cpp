//
// Copyright 2021-2022 Patrick Flynn
// This file is part of the Tiny Lang compiler.
// Tiny Lang is licensed under the BSD-3 license. See the COPYING file for more information.
//
// Structure.cpp
// Handles parsing for structs
#include <map>

#include <parser/Parser.hpp>
#include <ast.hpp>

// Parses and builds a structure
bool Parser::buildStruct() {
    Token token = scanner->getNext();
    std::string name = token.id_val;
    
    if (token.type != Id) {
        syntax->addError(scanner->getLine(), "Expected name for struct.");
        return false;
    }
    
    // Next token should be "is"
    token = scanner->getNext();
    if (token.type != Is) {
        syntax->addError(scanner->getLine(), "Expected \"is\".");
    }
    
    // Builds the struct items
    AstStruct *str = new AstStruct(name);
    token = scanner->getNext();
    
    while (token.type != End && token.type != Eof) {
        if (!buildStructMember(str, token)) return false;
        token = scanner->getNext();
    }
    
    tree->addStruct(str);
    
    return true;
}

bool Parser::buildStructMember(AstStruct *str, Token token) {
    std::string valName = token.id_val;
    
    if (token.type != Id) {
        syntax->addError(scanner->getLine(), "Expected id value.");
        token.print();
        return false;
    }
        
    // Get the data type
    token = scanner->getNext();
    if (token.type != Colon) {
        syntax->addError(scanner->getLine(), "Expected \':\' in structure member.");
        token.print();
        return false;
    }
    
    token = scanner->getNext();
    DataType dataType = DataType::Void;
        
    switch (token.type) {
        case Bool: dataType = DataType::Bool; break;
        case Char: dataType = DataType::Char; break;
        case I8: dataType = DataType::I8; break;
        case U8: dataType = DataType::U8; break;
        case I16: dataType = DataType::I16; break;
        case U16: dataType = DataType::U16; break;
        case I32: dataType = DataType::I32; break;
        case U32: dataType = DataType::U32; break;
        case I64: dataType = DataType::I64; break;
        case U64: dataType = DataType::U64; break;
        case Str: dataType = DataType::String; break;
        
        default: {}
    }
        
    // If its an array, build that. Otherwise, build the default value
    token = scanner->getNext();
        
    if (token.type == LBracket) {
        AstExpression *expr = buildExpression(DataType::I32, RBracket, true);
        if (!expr) return false;
                
        token = scanner->getNext();
        if (token.type != SemiColon) {
            syntax->addError(scanner->getLine(), "Expected terminator.");
            return false;
        }
            
        Var v;
        v.name = valName;
        v.type = DataType::Ptr;
        v.subType = dataType;
            
        str->addItem(v, expr);
    } else if (token.type == Assign) {
        AstExpression *expr = nullptr;
        expr = buildExpression(dataType, SemiColon, true);
        if (!expr) return false;
                
        Var v;
        v.name = valName;
        v.type = dataType;
        str->addItem(v, expr);
    } else {
        syntax->addError(scanner->getLine(), "Expected default value.");
        token.print();
        return false;
    }
        
    return true;
}

bool Parser::buildStructDec(AstBlock *block) {
    Token token = scanner->getNext();
    std::string name = token.id_val;
    
    if (token.type != Id) {
        syntax->addError(scanner->getLine(), "Expected structure name.");
        return false;
    }
    
    token = scanner->getNext();
    if (token.type != Colon) {
        syntax->addError(scanner->getLine(), "Expected \':\'");
        return false;
    }
    
    token = scanner->getNext();
    std::string structName = token.id_val;
    
    if (token.type != Id) {
        syntax->addError(scanner->getLine(), "Expected structure type.");
        return false;
    }
    
    // Make sure the given structure exists
    AstStruct *str = nullptr;
    
    for (auto s : tree->getStructs()) {
        if (s->getName() == structName) {
            str = s;
            break;
        }    
    }
    
    if (str == nullptr) {
        syntax->addError(scanner->getLine(), "Unknown structure.");
        return false;
    }
    
    // Now build the declaration and push back
    vars.push_back(name);
    AstStructDec *dec = new AstStructDec(name, structName);
    block->addStatement(dec);
    
    // Final syntax check
    token = scanner->getNext();
    if (token.type == SemiColon) {
        return true;
    } else if (token.type == Assign) {
        dec->setNoInit(true);
        AstVarAssign *empty = new AstVarAssign(name);
        AstExpression *arg = buildExpression(DataType::Struct);
        if (!arg) return false;
        empty->setExpression(arg);
        block->addStatement(empty);
        
        // TODO: The body should only be a function call expression or an ID
        // Do a syntax check
    }
    
    return true;
}

//
// Builds a structure assignment
// Despite the name, it also handles class calls since the syntax is similar
//
// TODO: SHould we get our own syntax?
//
bool Parser::buildStructAssign(AstBlock *block, Token idToken) {
    DataType dataType = typeMap[idToken.id_val].second;
    DataType ptrType = DataType::Void;
    
    AstExpression *expr = buildExpression(DataType::Void);
    if (!expr) return false;
    
    AstExprStatement *stmt = new AstExprStatement;
    stmt->setDataType(DataType::Void, ptrType);
    stmt->setExpression(expr);
    block->addStatement(stmt);
    
    return true;
    
    /*Token token = scanner->getNext();
    std::string member = token.id_val;
    
    if (token.type != Id) {
        syntax->addError(scanner->getLine(), "Expected structure member.");
        return false;
    }
    
    token = scanner->getNext();
    if (token.type == Assign) {
        AstStructAssign *sa = new AstStructAssign(idToken.id_val, member);
        block->addStatement(sa);
        
        // Get the data type of the member
        DataType memberType = DataType::Void;
        sa->setMemberType(memberType);
        
        for (AstStruct *str : tree->getStructs()) {
            for (Var item : str->getItems()) {
                if (item.name == member) {
                    memberType = item.type;
                    break;
                }
            }
        }
        
        AstExpression *arg = buildExpression(memberType);
        if (!arg) return false;
        sa->setExpression(arg);
    } else {
        syntax->addError(scanner->getLine(), "Invalid structure or class operation.");
        return false;
    }
    
    return true;*/
}


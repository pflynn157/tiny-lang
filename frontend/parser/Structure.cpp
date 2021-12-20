//
// Copyright 2021 Patrick Flynn
// This file is part of the Orka compiler.
// Orka is licensed under the BSD-3 license. See the COPYING file for more information.
//
// Structure.cpp
// Handles parsing for enums and structs
#include <map>

#include <parser/Parser.hpp>
#include <ast.hpp>

// Parses and builds an enumeration
bool Parser::buildEnum() {
    Token token = scanner->getNext();
    std::string name = token.id_val;
    
    if (token.type != Id) {
        syntax->addError(scanner->getLine(), "Expected enum name.");
        return false;
    }
    
    // See if we have a type for the enum. Default is int
    token = scanner->getNext();
    DataType dataType = DataType::Int32;
    bool useDefault = false;
    
    switch (token.type) {
        case Bool: dataType = DataType::Bool; break;
        case Char: dataType = DataType::Char; break;
        case Byte: dataType = DataType::Byte; break;
        case UByte: dataType = DataType::UByte; break;
        case Short: dataType = DataType::Short; break;
        case UShort: dataType = DataType::UShort; break;
        case Int: dataType = DataType::Int32; break;
        case UInt: dataType = DataType::UInt32; break;
        case Int64: dataType = DataType::Int64; break;
        case UInt64: dataType = DataType::UInt64; break;
        case Str: dataType = DataType::String; break;
        
        case Is: useDefault = true; break;
        
        default: {
            syntax->addError(scanner->getLine(), "Unknown token in enum declaration");
            return false;
        }
    }
    
    // Syntax check
    if (!useDefault) {
        token = scanner->getNext();
        if (token.type != Is) {
            syntax->addError(scanner->getLine(), "Expected \"is\"");
            return false;
        }
    }
    
    // Loop and get all the values
    std::map<std::string, AstExpression *> values;
    int index = 0;
    
    while (token.type != End && token.type != Eof) {
        token = scanner->getNext();
        std::string valName = token.id_val;
        
        if (token.type != Id) {
            syntax->addError(scanner->getLine(), "Expected enum value.");
            token.print();
            return false;
        }
        
        token = scanner->getNext();
        AstExpression *value = nullptr;
        
        if (token.type == Assign) {
        
        } else if (token.type != Comma && token.type != End) {
            syntax->addError(scanner->getLine(), "Unknown token in enum.");
            token.print();
            return false;
        }
        
        if (value == nullptr) {
            value = checkExpression(new AstInt(index), dataType);
            ++index;
        }
        
        values[valName] = value;
    }
    
    // Put it all together
    EnumDec theEnum;
    theEnum.name = name;
    theEnum.type = dataType;
    theEnum.values = values;
    enums[name] = theEnum;
    
    return true;
}

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
        syntax->addError(scanner->getLine(), "Expected \':\'.");
        return false;
    }
    
    token = scanner->getNext();
    DataType dataType = DataType::Void;
        
    switch (token.type) {
        case Bool: dataType = DataType::Bool; break;
        case Char: dataType = DataType::Char; break;
        case Byte: dataType = DataType::Byte; break;
        case UByte: dataType = DataType::UByte; break;
        case Short: dataType = DataType::Short; break;
        case UShort: dataType = DataType::UShort; break;
        case Int: dataType = DataType::Int32; break;
        case UInt: dataType = DataType::UInt32; break;
        case Int64: dataType = DataType::Int64; break;
        case UInt64: dataType = DataType::UInt64; break;
        case Str: dataType = DataType::String; break;
        
        case Id: {
            if (enums.find(token.id_val) != enums.end()) {
                EnumDec dec = enums[token.id_val];
                dataType = dec.type;
            }
        } break;
        
        default: {}
    }
        
    // If its an array, build that. Otherwise, build the default value
    token = scanner->getNext();
        
    if (token.type == LBracket) {
        AstExpression *expr = nullptr;
        if (!buildExpression(nullptr, DataType::Int32, RBracket, EmptyToken, &expr, true))
            return false;
                
        token = scanner->getNext();
        if (token.type != SemiColon) {
            syntax->addError(scanner->getLine(), "Expected terminator.");
            return false;
        }
            
        Var v;
        v.name = valName;
        v.type = DataType::Array;
        v.subType = dataType;
            
        str->addItem(v, expr);
    } else if (token.type == Assign) {
        AstExpression *expr = nullptr;
        if (!buildExpression(nullptr, dataType, SemiColon, EmptyToken, &expr, true))
            return false;
                
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
    AstStructDec *dec = new AstStructDec(name, structName);
    block->addStatement(dec);
    
    // Final syntax check
    token = scanner->getNext();
    if (token.type == SemiColon) {
        return true;
    } else if (token.type == Assign) {
        dec->setNoInit(true);
        AstVarAssign *empty = new AstVarAssign(name);
        if (!buildExpression(empty, DataType::Struct)) return false;
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
    Token token = scanner->getNext();
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
        
        if (!buildExpression(sa, memberType)) return false;
    } else {
        syntax->addError(scanner->getLine(), "Invalid structure or class operation.");
        return false;
    }
    
    return true;
}

//
// Copyright 2021 Patrick Flynn
// This file is part of the Orka compiler.
// Orka is licensed under the BSD-3 license. See the COPYING file for more information.
//
#include <iostream>

#include <ast.hpp>

std::string printDataType(DataType dataType) {
    switch (dataType) {
        case DataType::Void: return "void";
        case DataType::Bool: return "bool";
        case DataType::Char: return "char";
        case DataType::I8: return "i8";
        case DataType::U8: return "u8";
        case DataType::I16: return "i16";
        case DataType::U16: return "u16";
        case DataType::I32: return "i32";
        case DataType::U32: return "u32";
        case DataType::I64: return "i64";
        case DataType::U64: return "u64";
        case DataType::String: return "string";
        case DataType::Ptr: return "ptr";
        case DataType::Struct: return "struct";
    }
    return "";
}

void AstTree::print() {
    std::cout << "FILE: " << file << std::endl;
    std::cout << std::endl;
    
    for (auto str : structs) str->print();
    
    for (auto stmt : global_statements) {
        stmt->print();
    }
}

void AstExternFunction::print() {
    std::cout << "EXTERN FUNC " << name << "(";
    for (auto var : args) {
        std::cout << printDataType(var.type);
        if (var.subType != DataType::Void)
            std::cout << "*" << printDataType(var.subType);
        std::cout << ", ";
    }
    std::cout << ") ";
    std::cout << " -> " << printDataType(dataType);
    std::cout << std::endl;
}

void AstFunction::print() {
    std::cout << "FUNC " << name << "(";
    for (auto var : args) {
        std::cout << printDataType(var.type);
        if (var.subType != DataType::Void)
            std::cout << "*" << printDataType(var.subType);
        if (var.type == DataType::Struct)
            std::cout << "[" << var.typeName << "]";
        std::cout << ", ";
    }
    std::cout << ") -> ";
    std::cout << printDataType(dataType);
    std::cout << std::endl;
    
    for (auto stmt : block->getBlock()) {
        stmt->print();
        if (stmt->getExpressionCount()) {
            for (auto expr : stmt->getExpressions()) {
                for (int i = 0; i<8; i++) std::cout << " ";
                expr->print();
            }
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;
}

void AstStruct::print() {
    std::cout << "STRUCT " << name << std::endl;
    
    for (auto var : items) {
        std::cout << var.name << " : " << printDataType(var.type);
        if (var.subType != DataType::Void)
            std::cout << "*" << printDataType(var.subType);
        if (var.type == DataType::Struct)
            std::cout << "[" << var.typeName << "]";
        std::cout << " ";
        defaultExpressions[var.name]->print();
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void AstFuncCallStmt::print() {
    std::cout << "    ";
    std::cout << "FC " << name;
    std::cout << std::endl;
}

void AstReturnStmt::print() {
    std::cout << "    ";
    std::cout << "RETURN ";
    
    std::cout << std::endl;
}

void AstVarDec::print() {
    std::cout << "    ";
    std::cout << "VAR_DEC " << name << " : " << printDataType(dataType);
    if (ptrType != DataType::Void) {
        std::cout << "*" << printDataType(ptrType);
        std::cout << "[";
        size->print();
        std::cout << "]";
    }
    std::cout << std::endl;
}

void AstStructDec::print() {
    std::cout << "    ";
    std::cout << "STRUCT " << varName << " : " << structName;
    if (noInit) std::cout << " NOINIT";
    std::cout << std::endl;
}

void AstVarAssign::print() {
    std::cout << "    ";
    std::cout << "VAR= " << name << " : " << printDataType(dataType);
    if (ptrType != DataType::Void) {
        std::cout << "*" << printDataType(ptrType);
        std::cout << "[]";
    }
    std::cout << std::endl;
}

void AstArrayAssign::print() {
    std::cout << "    ";
    std::cout << "ARR= " << name;
    std::cout << std::endl;
}

void AstStructAssign::print() {
    std::cout << "    ";
    std::cout << "STRUCT= " << name << "." << member;
    std::cout << std::endl;
}

void AstIfStmt::print() {
    std::cout << "    ";
    std::cout << "IF " << std::endl;
    
    std::cout << "=========================" << std::endl;
    for (auto stmt : block->getBlock()) {
        stmt->print();
        if (stmt->getExpressionCount()) {
            for (auto expr : stmt->getExpressions()) {
                for (int i = 0; i<8; i++) std::cout << " ";
                expr->print();
            }
            std::cout << std::endl;
        }
    }
    
    std::cout << ">>" << std::endl;
    
    for (auto stmt : branches) {
        stmt->print();
        if (stmt->getExpressionCount()) {
            for (auto expr : stmt->getExpressions()) {
                for (int i = 0; i<8; i++) std::cout << " ";
                expr->print();
            }
            std::cout << std::endl;
        }
    }
    
    std::cout << "=========================" << std::endl;
}

void AstElifStmt::print() {
    std::cout << "    ";
    std::cout << "ELIF" << std::endl;
    
    std::cout << "-------------------------" << std::endl;
    for (auto stmt : block->getBlock()) {
        stmt->print();
        if (stmt->getExpressionCount()) {
            for (auto expr : stmt->getExpressions()) {
                for (int i = 0; i<8; i++) std::cout << " ";
                expr->print();
            }
            std::cout << std::endl;
        }
    }
    std::cout << "-------------------------" << std::endl;
}

void AstElseStmt::print() {
    std::cout << "    ";
    std::cout << "ELSE" << std::endl;
    
    std::cout << "-------------------------" << std::endl;
    for (auto stmt : block->getBlock()) {
        stmt->print();
        if (stmt->getExpressionCount()) {
            for (auto expr : stmt->getExpressions()) {
                for (int i = 0; i<8; i++) std::cout << " ";
                expr->print();
            }
            std::cout << std::endl;
        }
    }
    std::cout << "-------------------------" << std::endl;
}

void AstWhileStmt::print() {
    std::cout << "    ";
    std::cout << "WHILE" << std::endl;
    
    std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
    for (auto stmt : block->getBlock()) {
        stmt->print();
        if (stmt->getExpressionCount()) {
            for (auto expr : stmt->getExpressions()) {
                for (int i = 0; i<8; i++) std::cout << " ";
                expr->print();
            }
            std::cout << std::endl;
        }
    }
    std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
}

void AstBreak::print() {
    std::cout << "    ";
    std::cout << "BREAK" << std::endl;
}

void AstContinue::print() {
    std::cout << "    ";
    std::cout << "CONTINUE" << std::endl;
}

void AstEnd::print() {
    std::cout << "    ";
    std::cout << "END";
    
    std::cout << std::endl;
}

void AstNegOp::print() {
    std::cout << "(-";
    val->print();
    std::cout << ")";
}

void AstAddOp::print() {
    std::cout << "(";
    lval->print();
    std::cout << ") + (";
    rval->print();
    std::cout << ")";
}

void AstSubOp::print() {
    std::cout << "(";
    lval->print();
    std::cout << ") - (";
    rval->print();
    std::cout << ")";
}

void AstMulOp::print() {
    std::cout << "(";
    lval->print();
    std::cout << ") * (";
    rval->print();
    std::cout << ")";
}

void AstDivOp::print() {
    std::cout << "(";
    lval->print();
    std::cout << ") / (";
    rval->print();
    std::cout << ")";
}

void AstAndOp::print() {
    std::cout << "(";
    lval->print();
    std::cout << ") AND (";
    rval->print();
    std::cout << ")";
}

void AstOrOp::print() {
    std::cout << "(";
    lval->print();
    std::cout << ") OR (";
    rval->print();
    std::cout << ")";
}

void AstXorOp::print() {
    std::cout << "(";
    lval->print();
    std::cout << ") XOR (";
    rval->print();
    std::cout << ")";
}

void AstEQOp::print() {
    std::cout << "(";
    lval->print();
    std::cout << ") == (";
    rval->print();
    std::cout << ")";
}

void AstNEQOp::print() {
    std::cout << "(";
    lval->print();
    std::cout << ") != (";
    rval->print();
    std::cout << ")";
}

void AstGTOp::print() {
    std::cout << "(";
    lval->print();
    std::cout << ") > (";
    rval->print();
    std::cout << ")";
}

void AstLTOp::print() {
    std::cout << "(";
    lval->print();
    std::cout << ") < (";
    rval->print();
    std::cout << ")";
}

void AstGTEOp::print() {
    std::cout << "(";
    lval->print();
    std::cout << ") >= (";
    rval->print();
    std::cout << ")";
}

void AstLTEOp::print() {
    std::cout << "(";
    lval->print();
    std::cout << ") <= (";
    rval->print();
    std::cout << ")";
}

void AstChar::print() {
    std::cout << "CHAR(" << val << ")";
}

void AstByte::print() {
    std::cout << val;
}

void AstWord::print() {
    std::cout << val;
}

void AstInt::print() {
    std::cout << val;
}

void AstQWord::print() {
    std::cout << val;
}

void AstString::print() {
    std::cout << "\"" << val << "\"";
}

void AstID::print() {
    std::cout << val;
}

void AstArrayAccess::print() {
    std::cout << val << "[";
    index->print();
    std::cout << "]";
}

void AstStructAccess::print() {
    std::cout << var << "." << member;
}

void AstFuncCallExpr::print() {
    std::cout << name << "(";
    for (auto arg : args) {
        arg->print();
        std::cout << ", ";
    }
    std::cout << ")";
}


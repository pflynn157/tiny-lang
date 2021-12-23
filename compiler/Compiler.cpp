//
// Copyright 2021 Patrick Flynn
// This file is part of the Tiny Lang compiler.
// Tiny Lang is licensed under the BSD-3 license. See the COPYING file for more information.
//
#include "llvm/IR/BasicBlock.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;
using namespace llvm::sys;

#include <iostream>
#include <exception>

#include <Compiler.hpp>
#include <llvm-c/Support.h>

Compiler::Compiler(AstTree *tree, CFlags cflags) {
    char const *args[] = { "", "--x86-asm-syntax=intel" };
    LLVMParseCommandLineOptions(2, args, NULL);
    
    this->tree = tree;
    this->cflags = cflags;

    context = std::make_unique<LLVMContext>();
    mod = std::make_unique<Module>(cflags.name, *context);
    builder = std::make_unique<IRBuilder<>>(*context);
}

void Compiler::compile() {
    // Build the structures used by the program
    for (auto str : tree->getStructs()) {
        std::vector<Type *> elementTypes;
        
        for (auto v : str->getItems()) {
            Type *t = translateType(v.type, v.subType);
            elementTypes.push_back(t);
        }
        
        StructType *s = StructType::create(*context, elementTypes);
        s->setName(str->getName());
        
        structTable[str->getName()] = s;
        structElementTypeTable[str->getName()] = elementTypes;
    }

    // Build all other functions
    for (auto global : tree->getGlobalStatements()) {
        switch (global->getType()) {
            case AstType::Func: {
                symtable.clear();
                typeTable.clear();
                ptrTable.clear();
                
                compileFunction(global);
            } break;
            
            case AstType::ExternFunc: {
                compileExternFunction(global);
            } break;

            default: {}
        }
    }
}

void Compiler::debug() {
    mod->print(errs(), nullptr);
}

void Compiler::emitLLVM(std::string path) {
    std::error_code errorCode;
    raw_fd_ostream writer(path, errorCode);//, sys::fs::OF_None);
    
    mod->print(writer, NULL);
}

// Compiles an individual statement
void Compiler::compileStatement(AstStatement *stmt) {
    switch (stmt->getType()) {
        // A variable declaration (alloca) statement
        case AstType::VarDec: {
            AstVarDec *vd = static_cast<AstVarDec *>(stmt);
            Type *type = translateType(vd->getDataType(), vd->getPtrType());
            
            AllocaInst *var = builder->CreateAlloca(type);
            symtable[vd->getName()] = var;
            typeTable[vd->getName()] = vd->getDataType();
            ptrTable[vd->getName()] = vd->getPtrType();
        } break;
        
        // A structure declaration
        case AstType::StructDec: {
            AstStructDec *sd = static_cast<AstStructDec *>(stmt);
            StructType *type = structTable[sd->getStructName()];
            
            AllocaInst *var = builder->CreateAlloca(type);
            symtable[sd->getVarName()] = var;
            typeTable[sd->getVarName()] = DataType::Struct;
            structVarTable[sd->getVarName()] = sd->getStructName();
            
            AstStruct *str = nullptr;
            for (AstStruct *s : tree->getStructs()) {
                if (s->getName() == sd->getStructName()) {
                    str = s;
                    break;
                }
            }
            if (str == nullptr) return;
            
            // Init the elements
            if (!sd->isNoInit()) {
                int index = 0;
                for (Var member : str->getItems()) {
                    AstExpression *defaultExpr = str->getDefaultExpression(member.name);
                    Value *defaultVal = compileValue(defaultExpr, member.type);
                    
                    Value *ep = builder->CreateStructGEP(type, var, index);
                    builder->CreateStore(defaultVal, ep);
                    
                    ++index;
               }
            }
        } break;
        
        // A variable assignment
        case AstType::VarAssign: {
            AstVarAssign *va = static_cast<AstVarAssign *>(stmt);
            AllocaInst *ptr = symtable[va->getName()];
            DataType ptrType = typeTable[va->getName()];
            Value *val = compileValue(stmt->getExpressions().at(0), ptrType);
            
            builder->CreateStore(val, ptr);
        } break;
        
        // An array assignment
        case AstType::ArrayAssign: {
            AstArrayAssign *pa = static_cast<AstArrayAssign *>(stmt);
            Value *ptr = symtable[pa->getName()];
            DataType ptrType = typeTable[pa->getName()];
            DataType subType = ptrTable[pa->getName()];
            
            Value *index = compileValue(pa->getExpressions().at(0));
            Value *val = compileValue(pa->getExpressions().at(1), subType);
            
            if (ptrType == DataType::String) {
                PointerType *strPtrType = Type::getInt8PtrTy(*context);
                Type *i8Type = Type::getInt8Ty(*context);
                
                Value *arrayPtr = builder->CreateLoad(strPtrType, ptr);
                Value *ep = builder->CreateGEP(i8Type, arrayPtr, index);
                builder->CreateStore(val, ep);
            } else {
                DataType subType = ptrTable[pa->getName()];
                Type *arrayPtrType = translateType(ptrType, subType);
                Type *arrayElementType = translateType(subType);
                
                Value *ptrLd = builder->CreateLoad(arrayPtrType, ptr);
                Value *ep = builder->CreateGEP(arrayElementType, ptrLd, index);
                builder->CreateStore(val, ep);
            }
        } break;
        
        // A structure assignment
        case AstType::StructAssign: {
            AstStructAssign *sa = static_cast<AstStructAssign *>(stmt);
            Value *ptr = symtable[sa->getName()];
            int index = getStructIndex(sa->getName(), sa->getMember());
            
            Value *val = compileValue(sa->getExpressions().at(0), sa->getMemberType());
            
            std::string strTypeName = structVarTable[sa->getName()];
            StructType *strType = structTable[strTypeName];
            
            Value *structPtr = builder->CreateStructGEP(strType, ptr, index);
            builder->CreateStore(val, structPtr);
        } break;
        
        // Function call statements
        case AstType::FuncCallStmt: {
            compileFuncCallStatement(stmt);
        } break;
        
        // A return statement
        case AstType::Return: {
            compileReturnStatement(stmt);
        } break;
        
        // An IF statement
        case AstType::If: {
            compileIfStatement(stmt);
        } break;
        
        // A while loop
        case AstType::While: {
            compileWhileStatement(stmt);
        } break;
        
        // A break statement
        case AstType::Break: {
            builder->CreateBr(breakStack.top());
        } break;
        
        // A continue statement
        case AstType::Continue: {
            builder->CreateBr(continueStack.top());
        } break;
        
        default: {}
    }
}

// Converts an AST value to an LLVM value
Value *Compiler::compileValue(AstExpression *expr, DataType dataType) {
    switch (expr->getType()) {
        case AstType::I8L: {
            AstI8 *i8 = static_cast<AstI8 *>(expr);
            return builder->getInt8(i8->getValue());
        } break;
        
        case AstType::I16L: {
            AstI16 *i16 = static_cast<AstI16 *>(expr);
            return builder->getInt16(i16->getValue());
        } break;
        
        case AstType::I32L: {
            AstI32 *ival = static_cast<AstI32 *>(expr);
            return builder->getInt32(ival->getValue());
        } break;
        
        case AstType::I64L: {
            AstI64 *i64 = static_cast<AstI64 *>(expr);
            return builder->getInt64(i64->getValue());
        } break;
        
        case AstType::CharL: {
            AstChar *cval = static_cast<AstChar *>(expr);
            return builder->getInt8(cval->getValue());
        } break;
        
        case AstType::StringL: {
            AstString *str = static_cast<AstString *>(expr);
            return builder->CreateGlobalStringPtr(str->getValue());
        } break;
        
        case AstType::ID: {
            AstID *id = static_cast<AstID *>(expr);
            AllocaInst *ptr = symtable[id->getValue()];
            Type *type = translateType(typeTable[id->getValue()], ptrTable[id->getValue()]);
            
            if (typeTable[id->getValue()] == DataType::Struct) return ptr;
            return builder->CreateLoad(type, ptr);
        } break;
        
        case AstType::ArrayAccess: {
            AstArrayAccess *acc = static_cast<AstArrayAccess *>(expr);
            AllocaInst *ptr = symtable[acc->getValue()];
            DataType ptrType = typeTable[acc->getValue()];
            Value *index = compileValue(acc->getIndex());
            
            if (ptrType == DataType::String) {
                PointerType *strPtrType = Type::getInt8PtrTy(*context);
                Type *i8Type = Type::getInt8Ty(*context);
                
                Value *arrayPtr = builder->CreateLoad(strPtrType, ptr);
                Value *ep = builder->CreateGEP(i8Type, arrayPtr, index);
                return builder->CreateLoad(i8Type, ep);
            } else {
                DataType subType = ptrTable[acc->getValue()];
                Type *arrayPtrType = translateType(ptrType, subType);
                Type *arrayElementType = translateType(subType);
                
                Value *ptrLd = builder->CreateLoad(arrayPtrType, ptr);
                Value *ep = builder->CreateGEP(arrayElementType, ptrLd, index);
                return builder->CreateLoad(arrayElementType, ep);
            }
        } break;

        case AstType::StructAccess: {
            AstStructAccess *sa = static_cast<AstStructAccess *>(expr);
            AllocaInst *ptr = symtable[sa->getName()];
            int pos = getStructIndex(sa->getName(), sa->getMember());
            
            std::string strTypeName = structVarTable[sa->getName()];
            StructType *strType = structTable[strTypeName];
            Type *elementType = structElementTypeTable[strTypeName][pos];

            Value *ep = builder->CreateStructGEP(strType, ptr, pos);
            return builder->CreateLoad(elementType, ep);
        } break;
        
        case AstType::FuncCallExpr: {
            AstFuncCallExpr *fc = static_cast<AstFuncCallExpr *>(expr);
            std::vector<Value *> args;
            
            for (auto stmt : fc->getArguments()) {
                Value *val = compileValue(stmt);
                args.push_back(val);
            }
            
            Function *callee = mod->getFunction(fc->getName());
            if (!callee) std::cerr << "Invalid function call statement." << std::endl;
            return builder->CreateCall(callee, args);
        } break;
        
        case AstType::Neg: {
            AstNegOp *op = static_cast<AstNegOp *>(expr);
            Value *val = compileValue(op->getVal(), dataType);
            
            return builder->CreateNeg(val);
        } break;
        
        case AstType::Add:
        case AstType::Sub: 
        case AstType::Mul:
        case AstType::Div:
        case AstType::And:
        case AstType::Or:
        case AstType::Xor:
        case AstType::EQ:
        case AstType::NEQ:
        case AstType::GT:
        case AstType::LT:
        case AstType::GTE:
        case AstType::LTE: {
            AstBinaryOp *op = static_cast<AstBinaryOp *>(expr);
            AstExpression *lvalExpr = op->getLVal();
            AstExpression *rvalExpr = op->getRVal();
            
            Value *lval = compileValue(lvalExpr, dataType);
            Value *rval = compileValue(rvalExpr, dataType);
            
            bool strOp = false;
            bool rvalStr = false;
            
            if (lvalExpr->getType() == AstType::StringL || rvalExpr->getType() == AstType::StringL) {
                strOp = true;
                rvalStr = true;
            } else if (lvalExpr->getType() == AstType::StringL && rvalExpr->getType() == AstType::CharL) {
                strOp = true;
            } else if (lvalExpr->getType() == AstType::ID && rvalExpr->getType() == AstType::CharL) {
                AstID *lvalID = static_cast<AstID *>(lvalExpr);
                if (typeTable[lvalID->getValue()] == DataType::String) strOp = true;
            } else if (lvalExpr->getType() == AstType::ID && rvalExpr->getType() == AstType::ID) {
                AstID *lvalID = static_cast<AstID *>(lvalExpr);
                AstID *rvalID = static_cast<AstID *>(rvalExpr);
                
                if (typeTable[lvalID->getValue()] == DataType::String) strOp = true;
                if (typeTable[rvalID->getValue()] == DataType::String) {
                    strOp = true;
                    rvalStr = true;
                } else if (typeTable[rvalID->getValue()] == DataType::Char ||
                           typeTable[rvalID->getValue()] == DataType::I8) {
                    strOp = true;          
                }
            }
            
            // Build a string comparison if necessary
            if (strOp) {
                std::vector<Value *> args;
                args.push_back(lval);
                args.push_back(rval);
            
                if (op->getType() == AstType::EQ || op->getType() == AstType::NEQ) {
                    Function *strcmp = mod->getFunction("stringcmp");
                    if (!strcmp) std::cerr << "Error: Corelib function \"stringcmp\" not found." << std::endl;
                    Value *strcmpCall = builder->CreateCall(strcmp, args);
                    
                    int cmpVal = 0;
                    if (op->getType() == AstType::NEQ) cmpVal = 0;
                    Value *cmpValue = builder->getInt32(cmpVal);
                    
                    return builder->CreateICmpEQ(strcmpCall, cmpValue);
                } else if (op->getType() == AstType::Add) {
                    if (rvalStr) {
                        Function *callee = mod->getFunction("strcat_str");
                        if (!callee) std::cerr << "Error: corelib function \"strcat_str\" not found." << std::endl;
                        return builder->CreateCall(callee, args);
                    } else {
                        Function *callee = mod->getFunction("strcat_char");
                        if (!callee) std::cerr << "Error: corelib function \"strcat_char\" not found." << std::endl;
                        return builder->CreateCall(callee, args);
                    }
                } else {
                    // Invalid
                    return nullptr;
                }
            }
            
            // Otherwise, build a normal comparison
            switch (expr->getType()) {
                case AstType::Add: return builder->CreateAdd(lval, rval);
                case AstType::Sub: return builder->CreateSub(lval, rval);
                case AstType::Mul: return builder->CreateMul(lval, rval);
                case AstType::Div: return builder->CreateSDiv(lval, rval);
                
                case AstType::And: return builder->CreateAnd(lval, rval);
                case AstType::Or:  return builder->CreateOr(lval, rval);
                case AstType::Xor: return builder->CreateXor(lval, rval);
                    
                case AstType::EQ: return builder->CreateICmpEQ(lval, rval);
                case AstType::NEQ: return builder->CreateICmpNE(lval, rval);
                case AstType::GT: return builder->CreateICmpSGT(lval, rval);
                case AstType::LT: return builder->CreateICmpSLT(lval, rval);
                case AstType::GTE: return builder->CreateICmpSGE(lval, rval);
                case AstType::LTE: return builder->CreateICmpSLE(lval, rval);
                    
                default: {}
            }
        } break;
        
        default: {}
    }
    
    return nullptr;
}

Type *Compiler::translateType(DataType dataType, DataType subType, std::string typeName) {
    Type *type;
            
    switch (dataType) {
        case DataType::Char:
        case DataType::I8:
        case DataType::U8: type = Type::getInt8Ty(*context); break;
        
        case DataType::I16:
        case DataType::U16: type = Type::getInt16Ty(*context); break;
        
        case DataType::Bool:
        case DataType::I32:
        case DataType::U32: type = Type::getInt32Ty(*context); break;
        
        case DataType::I64:
        case DataType::U64: type = Type::getInt64Ty(*context); break;
        
        case DataType::String: type = Type::getInt8PtrTy(*context); break;
        
        case DataType::Ptr: {
            switch (subType) {
                case DataType::Char:
                case DataType::I8:
                case DataType::U8: type = Type::getInt8PtrTy(*context); break;
                
                case DataType::I16:
                case DataType::U16: type = Type::getInt16PtrTy(*context); break;
                
                case DataType::I32:
                case DataType::U32: type = Type::getInt32PtrTy(*context); break;
                
                case DataType::I64:
                case DataType::U64: type = Type::getInt64PtrTy(*context); break;
                
                case DataType::String: type = PointerType::getUnqual(Type::getInt8PtrTy(*context)); break;
                
                default: {}
            }
        } break;
        
        case DataType::Struct: {
            return structTable[typeName];
        } break;
        
        default: type = Type::getVoidTy(*context);
    }
    
    return type;
}

int Compiler::getStructIndex(std::string name, std::string member) {
    std::string name2 = structVarTable[name];
    if (name2 != "") name = name2;
    
    for (auto s : tree->getStructs()) {
        if (s->getName() != name) continue;

        std::vector<Var> members = s->getItems();
        for (int i = 0; i<members.size(); i++) {
            if (members.at(i).name == member) return i;
        }
    }

    return 0;
}


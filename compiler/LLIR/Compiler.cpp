//
// Copyright 2021 Patrick Flynn
// This file is part of the Tiny Lang compiler.
// Tiny Lang is licensed under the BSD-3 license. See the COPYING file for more information.
//

#include <iostream>
#include <exception>

#include <LLIR/Compiler.hpp>

Compiler::Compiler(AstTree *tree, CFlags cflags) {
    this->tree = tree;
    this->cflags = cflags;

    mod = new LLIR::Module(cflags.name);
    builder = new LLIR::IRBuilder(mod);
}

void Compiler::compile() {
    // Build the structures used by the program
    /*for (auto str : tree->getStructs()) {
        std::vector<Type *> elementTypes;
        
        for (auto v : str->getItems()) {
            Type *t = translateType(v.type, v.subType);
            elementTypes.push_back(t);
        }
        
        StructType *s = StructType::create(*context, elementTypes);
        s->setName(str->getName());
        
        structTable[str->getName()] = s;
        structElementTypeTable[str->getName()] = elementTypes;
    }*/

    // Build all other functions
    for (auto global : tree->getGlobalStatements()) {
        switch (global->getType()) {
            case AstType::Func: {
                //symtable.clear();
                //typeTable.clear();
                //ptrTable.clear();
                
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
    //mod->print(errs(), nullptr);
    mod->print();
}

// Compiles an individual statement
void Compiler::compileStatement(AstStatement *stmt) {
    switch (stmt->getType()) {
        // A variable declaration (alloca) statement
        case AstType::VarDec: {
            AstVarDec *vd = static_cast<AstVarDec *>(stmt);
            LLIR::Type *type = translateType(vd->getDataType(), vd->getPtrType());
            
            LLIR::Reg *var = builder->createAlloca(type);
            symtable[vd->getName()] = var;
            typeTable[vd->getName()] = vd->getDataType();
            ptrTable[vd->getName()] = vd->getPtrType();
        } break;
        
        // A structure declaration
        case AstType::StructDec: {
            AstStructDec *sd = static_cast<AstStructDec *>(stmt);
            /*StructType *type = structTable[sd->getStructName()];
            
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
            }*/
        } break;
        
        // A variable assignment
        case AstType::VarAssign: {
            AstVarAssign *va = static_cast<AstVarAssign *>(stmt);
            LLIR::Reg *ptr = symtable[va->getName()];
            DataType ptrType = typeTable[va->getName()];
            LLIR::Operand *val = compileValue(stmt->getExpressions().at(0), ptrType);
            LLIR::Type *type = translateType(ptrType);
            
            builder->createStore(type, val, ptr);
        } break;
        
        // An array assignment
        case AstType::ArrayAssign: {
            AstArrayAssign *pa = static_cast<AstArrayAssign *>(stmt);
            LLIR::Operand *ptr = symtable[pa->getName()];
            DataType ptrType = typeTable[pa->getName()];
            DataType subType = ptrTable[pa->getName()];
            
            LLIR::Operand *index = compileValue(pa->getExpressions().at(0));
            LLIR::Operand *val = compileValue(pa->getExpressions().at(1), subType);
            
            if (ptrType == DataType::String) {
            } else {
                LLIR::Type *arrayPtrType = translateType(ptrType, subType);
                LLIR::Type *arrayElementType = translateType(subType);
                
                LLIR::Operand *ptrLd = builder->createLoad(arrayPtrType, ptr);
                LLIR::Operand *ep = builder->createGEP(arrayPtrType, ptrLd, index);
                builder->createStore(arrayElementType, val, ep);
            }
        } break;
        
        // A structure assignment
        case AstType::StructAssign: {
            AstStructAssign *sa = static_cast<AstStructAssign *>(stmt);
            /*Value *ptr = symtable[sa->getName()];
            int index = getStructIndex(sa->getName(), sa->getMember());
            
            Value *val = compileValue(sa->getExpressions().at(0), sa->getMemberType());
            
            std::string strTypeName = structVarTable[sa->getName()];
            StructType *strType = structTable[strTypeName];
            
            Value *structPtr = builder->CreateStructGEP(strType, ptr, index);
            builder->CreateStore(val, structPtr);*/
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
            builder->createBr(breakStack.top());
        } break;
        
        // A continue statement
        case AstType::Continue: {
            builder->createBr(continueStack.top());
        } break;
        
        default: {}
    }
}

// Converts an AST value to an LLVM value
LLIR::Operand *Compiler::compileValue(AstExpression *expr, DataType dataType, LLIR::Block *destBlock) {
    LLIR::Type *type = translateType(dataType);

    switch (expr->getType()) {
        case AstType::I8L: {
            AstI8 *i8 = static_cast<AstI8 *>(expr);
            return builder->createI8(i8->getValue());
        } break;
        
        case AstType::I16L: {
            AstI16 *i16 = static_cast<AstI16 *>(expr);
            return builder->createI16(i16->getValue());
        } break;
        
        case AstType::I32L: {
            AstI32 *ival = static_cast<AstI32 *>(expr);
            return builder->createI32(ival->getValue());
        } break;
        
        case AstType::I64L: {
            AstI64 *i64 = static_cast<AstI64 *>(expr);
            return builder->createI64(i64->getValue());
        } break;
        
        case AstType::CharL: {
            AstChar *cval = static_cast<AstChar *>(expr);
            return builder->createI8(cval->getValue());
        } break;
        
        case AstType::StringL: {
            AstString *str = static_cast<AstString *>(expr);
            return builder->createString(str->getValue());
        } break;
        
        case AstType::ID: {
            AstID *id = static_cast<AstID *>(expr);
            LLIR::Reg *ptr = symtable[id->getValue()];
            LLIR::Type *type = translateType(typeTable[id->getValue()], ptrTable[id->getValue()]);
            
            if (typeTable[id->getValue()] == DataType::Struct) return ptr;
            return builder->createLoad(type, ptr);
        } break;
        
        case AstType::ArrayAccess: {
            AstArrayAccess *acc = static_cast<AstArrayAccess *>(expr);
            LLIR::Reg *ptr = symtable[acc->getValue()];
            DataType ptrType = typeTable[acc->getValue()];
            LLIR::Operand *index = compileValue(acc->getIndex());
            
            if (ptrType == DataType::String) {
                /*PointerType *strPtrType = Type::getInt8PtrTy(*context);
                Type *i8Type = Type::getInt8Ty(*context);
                
                Value *arrayPtr = builder->CreateLoad(strPtrType, ptr);
                Value *ep = builder->CreateGEP(i8Type, arrayPtr, index);
                return builder->CreateLoad(i8Type, ep);*/
            } else {
                DataType subType = ptrTable[acc->getValue()];
                LLIR::Type *arrayPtrType = translateType(ptrType, subType);
                LLIR::Type *arrayElementType = translateType(subType);
                
                LLIR::Operand *ptrLd = builder->createLoad(arrayPtrType, ptr);
                LLIR::Operand *ep = builder->createGEP(arrayPtrType, ptrLd, index);
                return builder->createLoad(arrayElementType, ep);
            }
        } break;

        /*case AstType::StructAccess: {
            AstStructAccess *sa = static_cast<AstStructAccess *>(expr);
            AllocaInst *ptr = symtable[sa->getName()];
            int pos = getStructIndex(sa->getName(), sa->getMember());
            
            std::string strTypeName = structVarTable[sa->getName()];
            StructType *strType = structTable[strTypeName];
            Type *elementType = structElementTypeTable[strTypeName][pos];

            Value *ep = builder->CreateStructGEP(strType, ptr, pos);
            return builder->CreateLoad(elementType, ep);
        } break;*/
        
        case AstType::FuncCallExpr: {
            AstFuncCallExpr *fc = static_cast<AstFuncCallExpr *>(expr);
            std::vector<LLIR::Operand *> args;
            
            for (auto stmt : fc->getArguments()) {
                LLIR::Operand *val = compileValue(stmt);
                args.push_back(val);
            }
            
            return builder->createCall(type, fc->getName(), args);
        } break;
        
        case AstType::Neg: {
            AstNegOp *op = static_cast<AstNegOp *>(expr);
            LLIR::Operand *val = compileValue(op->getVal(), dataType);
            
            return builder->createNeg(type, val);
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
        case AstType::LTE:
        case AstType::LogicalAnd:
        case AstType::LogicalOr: {
            AstBinaryOp *op = static_cast<AstBinaryOp *>(expr);
            AstExpression *lvalExpr = op->getLVal();
            AstExpression *rvalExpr = op->getRVal();
            
            // Do a type detection so we don't end up with null comparisons
            DataType dType = dataType;
            if (dataType == DataType::Void) {
                if (lvalExpr->getType() == AstType::ID) {
                    AstID *id = static_cast<AstID *>(lvalExpr);
                    type = translateType(typeTable[id->getValue()], ptrTable[id->getValue()]);
                    dType = typeTable[id->getValue()];
                } else if (lvalExpr->getType() == AstType::ID) {
                    AstID *id = static_cast<AstID *>(rvalExpr);
                    type = translateType(typeTable[id->getValue()], ptrTable[id->getValue()]);
                    dType = typeTable[id->getValue()];
                }
            }
            
            // Now, compile the operands
            LLIR::Operand *lval = compileValue(lvalExpr, dType);
            LLIR::Operand *rval = compileValue(rvalExpr, dType);
            
            /*bool strOp = false;
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
            }*/
            
            // Otherwise, build a normal comparison
            switch (expr->getType()) {
                case AstType::Add: return builder->createAdd(type, lval, rval);
                case AstType::Sub: return builder->createSub(type, lval, rval);
                case AstType::Mul: return builder->createSMul(type, lval, rval);
                case AstType::Div: return builder->createSDiv(type, lval, rval);
                
                case AstType::And: return builder->createAnd(type, lval, rval);
                case AstType::Or:  return builder->createOr(type, lval, rval);
                case AstType::Xor: return builder->createXor(type, lval, rval);
                    
                case AstType::EQ: return builder->createBeq(type, lval, rval, destBlock);
                case AstType::NEQ: return builder->createBne(type, lval, rval, destBlock);
                case AstType::GT: return builder->createBgt(type, lval, rval, destBlock);
                case AstType::LT: return builder->createBlt(type, lval, rval, destBlock);
                case AstType::GTE: return builder->createBge(type, lval, rval, destBlock);
                case AstType::LTE: return builder->createBle(type, lval, rval, destBlock);
                
                //case AstType::LogicalAnd: return builder->CreateLogicalAnd(lval, rval);
                //case AstType::LogicalOr: return builder->CreateLogicalOr(lval, rval);
                    
                default: {}
            }
        } break;
        
        default: {}
    }
    
    return nullptr;
}

LLIR::Type *Compiler::translateType(DataType dataType, DataType subType, std::string typeName) {
    LLIR::Type *type;
            
    switch (dataType) {
        case DataType::Char:
        case DataType::I8:
        case DataType::U8: type = LLIR::Type::createI8Type(); break;
        
        case DataType::I16:
        case DataType::U16: type = LLIR::Type::createI16Type(); break;
        
        case DataType::Bool:
        case DataType::I32:
        case DataType::U32: type = LLIR::Type::createI32Type(); break;
        
        case DataType::I64:
        case DataType::U64: type = LLIR::Type::createI64Type(); break;
        
        case DataType::String: type = LLIR::PointerType::createI8PtrType(); break;
        
        case DataType::Ptr: {
            switch (subType) {
                case DataType::Void: type = LLIR::PointerType::createVoidPtrType(); break;
            
                case DataType::Char:
                case DataType::I8:
                case DataType::U8: type = LLIR::PointerType::createI8PtrType(); break;
                
                case DataType::I16:
                case DataType::U16: type = LLIR::PointerType::createI16PtrType(); break;
                
                case DataType::I32:
                case DataType::U32: type = LLIR::PointerType::createI32PtrType(); break;
                
                case DataType::I64:
                case DataType::U64: type = LLIR::PointerType::createI64PtrType(); break;
                
                //case DataType::String: type = PointerType::getUnqual(Type::getInt8PtrTy(*context)); break;
                
                default: {}
            }
        } break;
        
        /*case DataType::Struct: {
            return structTable[typeName];
        } break;*/
        
        default: type = LLIR::Type::createVoidType();
    }
    
    return type;
}

int Compiler::getStructIndex(std::string name, std::string member) {
    /*std::string name2 = structVarTable[name];
    if (name2 != "") name = name2;
    
    for (auto s : tree->getStructs()) {
        if (s->getName() != name) continue;

        std::vector<Var> members = s->getItems();
        for (int i = 0; i<members.size(); i++) {
            if (members.at(i).name == member) return i;
        }
    }
*/
    return 0;
}


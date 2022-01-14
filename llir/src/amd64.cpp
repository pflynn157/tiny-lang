#include <iostream>
#include <fstream>
#include <cstdlib>

#include <amd64.hpp>
#include <llir.hpp>

namespace LLIR {

Amd64Writer::Amd64Writer(Module *mod) {
    this->mod = mod;
    file = new X86File(mod->getName());
    
    // Init the register map
    regMap[0] = X86Reg::AX;
    regMap[1] = X86Reg::BX;
    regMap[2] = X86Reg::CX;
    regMap[3] = X86Reg::DX;
    
    // Init the arguments register map
    argRegMap[0] = X86Reg::DI;
    argRegMap[1] = X86Reg::SI;
    argRegMap[2] = X86Reg::DX;
    argRegMap[3] = X86Reg::CX;
    argRegMap[4] = X86Reg::R8;
    argRegMap[5] = X86Reg::R9;
}

void Amd64Writer::compile() {
    // Data section
    for (int i = 0; i<mod->getStringCount(); i++) {
        StringPtr *str = mod->getString(i);
        X86Data *d = new X86Data(str->getName(), str->getValue());
        file->addData(d);
    }
    
    // Text section
    for (int i = 0; i<mod->getFunctionCount(); i++) {
        Function *func = mod->getFunction(i);
        switch (func->getLinkage()) {
            case Linkage::Global: {
                X86GlobalFunc *x86Func = new X86GlobalFunc(func->getName());
                file->addCode(x86Func);
            } break;
            
            case Linkage::Local: {} break;
            
            case Linkage::Extern: {
                //assembly += ".extern " + func->getName() + "\n";
                continue;
            } break;
        }
        
        // Setup the stack
        X86Imm *stackImm = new X86Imm(0);
        
        X86Push *p = new X86Push(new X86Reg64(X86Reg::BP));
        file->addCode(p);
        X86Mov *mov = new X86Mov(new X86Reg64(X86Reg::BP), new X86Reg64(X86Reg::SP));
        file->addCode(mov);
        X86Sub *sub = new X86Sub(new X86Reg64(X86Reg::SP), stackImm);
        file->addCode(sub);
        
        // Blocks
        for (int j = 0; j<func->getBlockCount(); j++) {
            Block *block = func->getBlock(j);
            file->addCode(new X86Label(block->getName()));
            
            // Instructions
            for (int k = 0; k<block->getInstrCount(); k++) {
                compileInstruction(block->getInstruction(k));
            }
        }
        
        if (stackPos < 16) {
            stackImm->setValue(16);
        } else {
            int i = 16;
            for (; i < (stackPos + 4); i += 8) {}
            i += 8;
            stackImm->setValue(i);
        }
        stackPos = 0;
        
        // Clean up the stack and leave
        file->addCode(new X86Leave);
        file->addCode(new X86Ret);
    }
}

void Amd64Writer::compileInstruction(Instruction *instr) {
    switch (instr->getType()) {
        case InstrType::None: break;
        
        case InstrType::Ret: {
            Type *type = instr->getDataType();
            X86Operand *src = compileOperand(instr->getOperand1(), type);
            X86Operand *dest;
            switch (type->getType()) {
                case DataType::I8: break;
                case DataType::I16: break;
                case DataType::I32: dest = new X86Reg32(X86Reg::AX); break;
                case DataType::I64: dest = new X86Reg64(X86Reg::AX); break;
                
                default: {}
            }
            
            X86Mov *mov = new X86Mov(dest, src);
            file->addCode(mov);
        } break;
        
        case InstrType::RetVoid: break;    // Nothing on x86-64
        
        // Math
        case InstrType::Add:
        case InstrType::Sub:
        case InstrType::And:
        case InstrType::Or:
        case InstrType::Xor: {
            X86Operand *op1 = compileOperand(instr->getOperand1(), instr->getDataType());
            X86Operand *op2 = compileOperand(instr->getOperand2(), instr->getDataType());
            X86Operand *fop1, *fop2;
            if (op1->getType() == X86Type::Imm) {
                fop2 = op1;
                fop1 = op2;
            } else {
                fop1 = op1;
                fop2 = op2;
            }
            
            X86Instr *instr2;
            switch (instr->getType()) {
                case InstrType::Add: instr2 = new X86Add(fop1, fop2); break;
                case InstrType::Sub: instr2 = new X86Sub(fop1, fop2); break;
                case InstrType::And: instr2 = new X86And(fop1, fop2); break;
                case InstrType::Or: instr2 = new X86Or(fop1, fop2); break;
                case InstrType::Xor: instr2 = new X86Xor(fop1, fop2); break;
                
                default: {}
            }
            
            file->addCode(instr2);
            
            // Now, we need a move so we're in the right register
            // I love x86
            X86Operand *dest = compileOperand(instr->getDest(), instr->getDataType());
            X86Mov *mov = new X86Mov(dest, fop1);
            file->addCode(mov);
        } break;
        
        case InstrType::UMul: break;
        case InstrType::SMul: break;
        case InstrType::UDiv: break;
        case InstrType::SDiv: break;
        case InstrType::URem: break;
        case InstrType::SRem: break;
        
        case InstrType::Br: break;
        
        case InstrType::Call: {
            FunctionCall *fc = static_cast<FunctionCall *>(instr);
            
            int pos = 0;
            for (Operand *arg : fc->getArgs()) {
                // TODO: Some better argument detection for the registers would be ideal
                X86Operand *op = compileOperand(arg, Type::createI32Type());
                
                X86Reg regType = argRegMap[pos];
                ++pos;
                
                X86Operand *dest = new X86Reg32(regType);
                if (op->getType() == X86Type::String)
                    dest = new X86Reg64(regType);
                    
                X86Mov *mov = new X86Mov(dest, op);
                file->addCode(mov);
            }
            
            X86Call *call = new X86Call(fc->getName());
            file->addCode(call);
        } break;
        
        case InstrType::Alloca: {
            switch (instr->getDataType()->getType()) {
                case DataType::Void: break;
                case DataType::I8: stackPos += 1; break;
                case DataType::I16: stackPos += 2; break;
                case DataType::F32:
                case DataType::I32: stackPos += 4; break;
                case DataType::I64:
                case DataType::F64:
                case DataType::Ptr: stackPos += 8; break;
            }
            
            Mem *mem = static_cast<Mem *>(instr->getDest());
            memMap[mem->getName()] = stackPos;
        } break;
        
        case InstrType::Load: {
            X86Operand *src = compileOperand(instr->getOperand1(), instr->getDataType());
            X86Operand *dest = compileOperand(instr->getDest(), instr->getDataType());
            X86Mov *mov = new X86Mov(dest, src);
            file->addCode(mov);
        } break;
        
        case InstrType::Store: {
            X86Operand *src = compileOperand(instr->getOperand1(), instr->getDataType());
            X86Operand *dest = compileOperand(instr->getOperand2(), instr->getDataType());
            X86Mov *mov = new X86Mov(dest, src);
            file->addCode(mov);
        } break;
    }
}

X86Operand *Amd64Writer::compileOperand(Operand *src, Type *type) {
    switch (src->getType()) {
        // Return an immediate operand
        case OpType::Imm: {
            Imm *imm = static_cast<Imm *>(src);
            return new X86Imm(imm->getValue());
        }
        
        // Return a memory operand
        case OpType::Mem: {
            Mem *mem = static_cast<Mem *>(src);
            int pos = memMap[mem->getName()];
            X86Mem *mem2 = new X86Mem(new X86Imm(0 - pos));
            mem2->setSizeAttr(getSizeForType(type));
            return mem2;
        }
        
        // Return a hardware register
        case OpType::HReg: {
            HReg *reg = static_cast<HReg *>(src);
            X86Reg rType = regMap[reg->getNum()];
            
            switch (type->getType()) {
                case DataType::Void: break;
                case DataType::I8: break;
                case DataType::I16: break;
                case DataType::I32: return new X86Reg32(rType);
                case DataType::Ptr:
                case DataType::I64: return new X86Reg64(rType);
                case DataType::F32:
                case DataType::F64: break;
            }
        } break;
        
        // A string operand
        case OpType::String: {
            StringPtr *ptr = static_cast<StringPtr *>(src);
            return new X86String(ptr->getName());
        } break;
        
        default: return nullptr;
    }
    
    return nullptr;
}

void Amd64Writer::dump() {
    std::cout << file->print() << std::endl;
}

void Amd64Writer::writeToFile() {
    std::string path = "/tmp/" + mod->getName() + ".s";
    std::ofstream writer(path);
    writer << file->print() << std::endl;
    writer.close();
}

void Amd64Writer::writeToFile(std::string path) {
    std::ofstream writer(path);
    writer << file->print() << std::endl;
    writer.close();
}

// TODO: This function probably should be replaced
void Amd64Writer::build() {
    std::string asmPath = "/tmp/" + mod->getName() + ".s";
    std::string objPath = "/tmp/" + mod->getName() + ".o";
    std::string binPath = "/tmp/" + mod->getName();
    
    std::string cmd1 = "as " + asmPath + " -o " + objPath;
    std::string cmd2 = "gcc " + objPath + " -o " + binPath;
    
    system(cmd1.c_str());
    system(cmd2.c_str());
}

std::string Amd64Writer::getSizeForType(Type *type) {
    switch (type->getType()) {
        case DataType::Void: break;
        case DataType::I8: return "BYTE PTR";
        case DataType::I16: return "WORD PTR";
        case DataType::F32:
        case DataType::I32: return "DWORD PTR";
        case DataType::I64:
        case DataType::F64:
        case DataType::Ptr: return "QWORD PTR";
    }
    return "";
}

} // end namespace LLIR

#include <iostream>
#include <fstream>
#include <cstdlib>

#include <amd64.hpp>
#include <llir.hpp>

namespace LLIR {

Amd64Writer::Amd64Writer(Module *mod) {
    this->mod = mod;
    file = new X86File(mod->getName());
}

void Amd64Writer::compile() {
    // Data section
    
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
        X86Push *p = new X86Push(new X86Reg64(X86Reg::BP));
        file->addCode(p);
        X86Mov *mov = new X86Mov(new X86Reg64(X86Reg::BP), new X86Reg64(X86Reg::SP));
        file->addCode(mov);
        X86Sub *sub = new X86Sub(new X86Reg64(X86Reg::SP), new X86Imm(func->getStackSize()));
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
        
        // Clean up the stack and leave
        file->addCode(new X86Leave);
        file->addCode(new X86Ret);
    }
}

void Amd64Writer::compileInstruction(Instruction *instr) {
    switch (instr->getType()) {
        case InstrType::None: break;
        
        case InstrType::Ret: {
            Operand *src = instr->getOperand1();
            switch (src->getType()) {
                case OpType::Imm: break;
                
                case OpType::Reg: {
                    Reg *reg = static_cast<Reg *>(src);
                    if (regMap[reg->getName()] == "eax" || regMap[reg->getName()] == "rax") break;
                    // TODO: Either load memory or move to the proper register
                    
                } break;
            }
        } break;
        
        case InstrType::RetVoid: break;    // Nothing on x86-64
        
        case InstrType::Add: break;
        case InstrType::Sub: break;
        case InstrType::UMul: break;
        case InstrType::SMul: break;
        case InstrType::UDiv: break;
        case InstrType::SDiv: break;
        case InstrType::URem: break;
        case InstrType::SRem: break;
        
        case InstrType::Br: break;
        case InstrType::Beq: break;
        case InstrType::Bne: break;
        case InstrType::Bgt: break;
        case InstrType::Bge: break;
        case InstrType::Blt: break;
        case InstrType::Ble: break;
        
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
            
            Reg *reg = static_cast<Reg *>(instr->getDest());
            memMap[reg->getName()] = stackPos;
        } break;
        
        case InstrType::Load: {
            Reg *destReg = static_cast<Reg *>(instr->getDest());
            regMap[destReg->getName()] = "eax";
            
            Reg *srcReg = static_cast<Reg *>(instr->getOperand1());
            int pos = memMap[srcReg->getName()];
            
            /*assembly += "  mov eax, ";
            assembly += getSizeForType(instr->getDataType());
            assembly += " [rbp-" + std::to_string(pos) + "]\n";*/
        } break;
        
        case InstrType::Store: {
            Reg *dest = static_cast<Reg *>(instr->getOperand2());
            int pos = memMap[dest->getName()];
            
            Operand *src = instr->getOperand1();
            switch (src->getType()) {
                case OpType::Imm: {
                    Imm *imm = static_cast<Imm *>(src);
                    
                    /*assembly += "  mov " + getSizeForType(instr->getDataType());
                    assembly += " [rbp-" + std::to_string(pos) + "]";
                    assembly += ", " + std::to_string(imm->getValue()) + "\n";*/
                } break;
                
                case OpType::Reg: break;
            }
        } break;
    }
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

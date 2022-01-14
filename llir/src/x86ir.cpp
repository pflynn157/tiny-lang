#include <x86ir.hpp>

namespace LLIR {

std::string X86File::print(AsmType type) {
    std::string file = "";
    if (type == AsmType::GAS) {
        file += ".intel_syntax noprefix\n";
        
        // Data
        file += "\n";
        file += ".data\n";
        for (X86Data *ln : data) {
            file += ln->print() + "\n";
        }
        
        // Code
        file += "\n";
        file += ".text\n";
        
        for (X86Instr *ln : code) {
            switch (ln->getType()) {
                case X86Type::Label:
                case X86Type::Extern:
                case X86Type::Global: break;
                
                default: file += "  ";
            }
            file += ln->print() + "\n";
        }
    }
    
    return file;
}

std::string X86GlobalFunc::print() {
    std::string ret = "";
    ret += ".globl " + name + "\n";
    ret += ".type " + name + ",@function\n";
    ret += name + ":";
    return ret;
}

std::string X86Push::print() {
    return "push " + op1->print();
}

std::string X86Mov::print() {
    return "mov " + op1->print() + ", " + op2->print();
}

std::string X86Add::print() {
    return "add " + op1->print() + ", " + op2->print();
}

std::string X86Sub::print() {
    return "sub " + op1->print() + ", " + op2->print();
}

std::string X86Imm::print() {
    return std::to_string(value);
}

std::string X86Reg32::print() {
    switch (regType) {
        case X86Reg::AX: return "eax";
        case X86Reg::BX: return "ebx";
        case X86Reg::CX: return "ecx";
        case X86Reg::DX: return "edx";
        
        case X86Reg::SI: return "esi";
        case X86Reg::DI: return "edi";
        case X86Reg::SP: return "esp";
        case X86Reg::BP: return "ebp";
        
        case X86Reg::R8: return "r8d";
        case X86Reg::R9: return "r9d";
        case X86Reg::R10: return "r10d";
        case X86Reg::R11: return "r11d";
        case X86Reg::R12: return "r12d";
        case X86Reg::R13: return "r13d";
        case X86Reg::R14: return "r14d";
        case X86Reg::R15: return "r15d";
    }
    return "";
}

std::string X86Reg64::print() {
    switch (regType) {
        case X86Reg::AX: return "rax";
        case X86Reg::BX: return "rbx";
        case X86Reg::CX: return "rcx";
        case X86Reg::DX: return "rdx";
        
        case X86Reg::SI: return "rsi";
        case X86Reg::DI: return "rdi";
        case X86Reg::SP: return "rsp";
        case X86Reg::BP: return "rbp";
        
        case X86Reg::R8: return "r8";
        case X86Reg::R9: return "r9";
        case X86Reg::R10: return "r10";
        case X86Reg::R11: return "r11";
        case X86Reg::R12: return "r12";
        case X86Reg::R13: return "r13";
        case X86Reg::R14: return "r14";
        case X86Reg::R15: return "r15";
    }
    return "";
}

std::string X86Mem::print() {
    std::string dest =  sizeAttr + " [";
    dest += base->print();
    dest += offset->print();
    dest += "]";
    return dest;
}

std::string X86String::print() {
    return "OFFSET FLAT:" + value;
}

} // end namespace LLIR

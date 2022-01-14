#pragma once

#include <string>
#include <vector>

namespace LLIR {

//
// Enumerations
//
enum class X86Type {
    None,        // Always translates to NOP
    
    Label,
    Global,
    Extern,
    
    Push,
    Mov,
    Sub,
    Leave,
    Ret,
    
    Reg8,
    Reg8H,
    Reg16,
    Reg32,
    Reg64,
    
    Imm
};

enum class X86Reg {
    AX,
    BX,
    CX,
    DX,
    SI,
    DI,
    SP,
    BP,
    R8, R9, R10, R11, R12, R13, R14, R15
};

enum class AsmType {
    GAS,
    NASM
};

// Forward declarations
class X86Instr;
class X86Operand;

// Represents an x86 file
class X86File {
public:
    explicit X86File(std::string name) {
        this->name = name;
    }
    
    void addCode(X86Instr *c) { code.push_back(c); }
    
    std::string print(AsmType type = AsmType::GAS);
private:
    std::string name = "";
    std::vector<X86Instr *> code;
};

//
// Represents an X86 instruction
// This is the base of all X86 instructions
//
class X86Instr {
public:
    explicit X86Instr(X86Type type) {
        this->type = type;
    }
    
    void setOperand1(X86Operand *op1) { this->op1 = op1; }
    void setOperand2(X86Operand *op2) { this->op2 = op2; }
    
    X86Type getType() { return type; }
    X86Operand *getOperand1() { return op1; }
    X86Operand *getOperand2() { return op2; }
    
    virtual std::string print() { return "nop"; }
protected:
    X86Type type = X86Type::None;
    X86Operand *op1 = nullptr;
    X86Operand *op2 = nullptr;
};

// The labels
class X86Label : public X86Instr {
public:
    explicit X86Label(std::string name) : X86Instr(X86Type::Label) {
        this->name = name;
    }
    
    std::string print() {
        return name + ":";
    }
protected:
    std::string name = "";
};

// Globally visible functions
class X86GlobalFunc : public X86Instr {
public:
    explicit X86GlobalFunc(std::string name) : X86Instr(X86Type::Global) {
        this->name = name;
    }
    
    std::string print();
protected:
    std::string name = "";
};

// A push instruction
class X86Push : public X86Instr {
public:
    explicit X86Push() : X86Instr(X86Type::Push) {}
    explicit X86Push(X86Operand *op) : X86Instr(X86Type::Push) {
        op1 = op;
    }
    
    std::string print();
};

// An MOV instruction
class X86Mov : public X86Instr {
public:
    explicit X86Mov(X86Operand *op1, X86Operand *op2) : X86Instr(X86Type::Mov) {
        this->op1 = op1;
        this->op2 = op2;
    }
    
    std::string print();
};

// A SUB instruction
class X86Sub : public X86Instr {
public:
    explicit X86Sub(X86Operand *op1, X86Operand *op2) : X86Instr(X86Type::Sub) {
        this->op1 = op1;
        this->op2 = op2;
    }
    
    std::string print();
};

// a LEAVE instruction
class X86Leave : public X86Instr {
public:
    X86Leave() : X86Instr(X86Type::Leave) {}
    std::string print() { return "leave"; }
};

// a RET instruction
class X86Ret : public X86Instr {
public:
    X86Ret() : X86Instr(X86Type::Ret) {}
    std::string print() { return "ret"; }
};

//
// Represents an X86 operand
//
class X86Operand {
public:
    explicit X86Operand(X86Type type) {
        this->type = type;
    }
    
    X86Type getType() { return type; }
    
    virtual std::string print() { return ""; }
protected:
    X86Type type = X86Type::None;
};

// Represents an immediate value
class X86Imm : public X86Operand {
public:
    explicit X86Imm(uint64_t imm) : X86Operand(X86Type::Imm) {
        this->value = imm;
    }
    
    std::string print();
private:
    uint64_t value = 0;
};

// Represents a 64-bit register
class X86Reg64 : public X86Operand {
public:
    explicit X86Reg64(X86Reg regType) : X86Operand(X86Type::Reg64) {
        this->regType = regType;
    }
    
    X86Reg getType() { return regType; }
    std::string print();
protected:
    X86Reg regType;
};

} // end namespace llir

#pragma once

namespace LLIR {

// Represents operand types
enum class OpType {
    None,
    
    Imm,
    Reg,
    Label,
    String,
    
    Mem,
    HReg,        // Hardware register
    AReg,        // Argument register
    PReg         // Pointer register
};

//
// The base class for operands
//
class Operand {
public:
    explicit Operand(OpType type) {
        this->type = type;
    }
    
    OpType getType() { return type; }
    
    virtual void print() {}
protected:
    OpType type = OpType::None;
};

// Represents an immediate value
class Imm : public Operand {
public:
    explicit Imm(int64_t imm) : Operand(OpType::Imm) {
        this->imm = imm;
    }
    
    int64_t getValue() { return imm; }
    void setValue(int64_t imm) { this->imm = imm; }
    
    void print();
private:
    int64_t imm = 0;
};

// Represents a register
class Reg : public Operand {
public:
    explicit Reg(std::string name) : Operand(OpType::Reg) {
        this->name = name;
    }
    
    std::string getName() { return name; }
    
    void print();
private:
    std::string name = "";
};

// Represents a label
class Label : public Operand {
public:
    explicit Label(std::string name) : Operand(OpType::Label) {
        this->name = name;
    }
    
    std::string getName() { return name; }
    
    void print();
private:
    std::string name = "";
};

// Represents a string
class StringPtr : public Operand {
public:
    explicit StringPtr(std::string name, std::string val) : Operand(OpType::String) {
        this->name = name;
        this->val = val;
    }
    
    std::string getName() { return name; }
    std::string getValue() { return val; }
    
    void print();
private:
    std::string name = "";
    std::string val = "";
};

//
// NOTE: These are more hardware-specific operands, and should only be used by
// the transform layer and the assembly writers
//

// Represents a memory location
class Mem : public Operand {
public:
    explicit Mem(std::string name) : Operand(OpType::Mem) {
        this->name = name;
    }
    
    std::string getName() { return name; }
    
    void print();
private:
    std::string name = "";
};

// Represents a hardware register
class HReg : public Operand {
public:
    explicit HReg(int num) : Operand(OpType::HReg) {
        this->num = num;
    }
    
    int getNum() { return num; }
    
    void print();
private:
    int num = 0;
};

// Represents a hardware argument register
class AReg : public Operand {
public:
    explicit AReg(int num) : Operand(OpType::AReg) {
        this->num = num;
    }
    
    int getNum() { return num; }
    
    void print();
private:
    int num = 0;
};

// Represents a hardware pointer register
class PReg : public Operand {
public:
    explicit PReg(int num) : Operand(OpType::PReg) {
        this->num = num;
    }
    
    int getNum() { return num; }
    
    void print();
private:
    int num = 0;
};

} // end namespace LLIR

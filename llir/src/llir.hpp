#pragma once

#include <string>
#include <vector>

namespace LLIR {

// Enumerations
enum class DataType {
    Void,
    I8,
    I16,
    I32,
    I64,
    F32,
    F64,
    Ptr
};

enum class Linkage {
    Global,
    Local,
    Extern
};

enum class InstrType {
    None,
    
    // Returns
    Ret,
    RetVoid,
    
    // Integer math
    Add,
    Sub,
    SMul,
    UMul,
    SDiv,
    UDiv,
    SRem,
    URem,
    
    // Jumps
    // We're going to use RISC-V style because these will be the easiest
    // to translate on different architectures
    Br,
    Beq,
    Bne,
    Bgt,
    Bge,
    Blt,
    Ble,
    
    // Memory
    Alloca,
    Load,
    Store
};

// Represents operand types
enum class OpType {
    None,
    
    Imm,
    Reg,
    Label
};

// Forward declarations
class Function;
class Block;
class Instruction;
class Operand;

//
// The base of all LLIR projects
//
class Module {
public:
    explicit Module(std::string name) {
        this->name = name;
    }
    
    void addFunction(Function *func) { functions.push_back(func); }
    
    std::string getName() { return name; }
    int getFunctionCount() { return functions.size(); }
    Function *getFunction(int pos) { return functions.at(pos); }
    
    void print();
private:
    std::string name = "";
    std::vector<Function *> functions;
};

//
// Represents a data type
//
class Type {
public:
    explicit Type(DataType type) {
        this->type = type;
    }
    
    DataType getType() { return type; }
    
    void print();
protected:
    DataType type = DataType::Void;
};

//
// Represents a function
//
class Function {
public:
    explicit Function(std::string name, Linkage linkage) {
        this->name = name;
        this->linkage = linkage;
        dataType = new Type(DataType::Void);
    }
    
    void setDataType(Type *d) {
        if (dataType) delete dataType;
        dataType = d;
    }
    
    void addBlock(Block *block) { blocks.push_back(block); }
    void setStackSize(int size) { stackSize = size; }
    
    std::string getName() { return name; }
    Linkage getLinkage() { return linkage; }
    int getStackSize() { return stackSize; }
    
    int getBlockCount() { return blocks.size(); }
    Block *getBlock(int pos) { return blocks.at(pos); }
    
    void print();
private:
    Type *dataType;
    std::string name = "";
    Linkage linkage = Linkage::Local;
    std::vector<Block *> blocks;
    int stackSize = 0;
};

//
// Represents a global string value
//

//
// Represents a basic block
//
class Block {
public:
    explicit Block(std::string name) {
        this->name = name;
    }
    
    void addInstruction(Instruction *i) { instrs.push_back(i); }
    
    std::string getName() { return name; }
    
    int getInstrCount() { return instrs.size(); }
    Instruction *getInstruction(int pos) { return instrs.at(pos); }
    
    void print();
private:
    std::string name = "";
    std::vector<Instruction *> instrs;
};

//
// The base class for instructions
//
class Instruction {
public:
    explicit Instruction(InstrType type) {
        this->type = type;
        dataType = new Type(DataType::Void);
    }
    
    void setDataType(Type *d) {
        if (dataType) delete dataType;
        dataType = d;
    }
    
    void setDest(Operand *d) { dest = d; }
    void setOperand1(Operand *o) { src1 = o; }
    void setOperand2(Operand *o) { src2 = o; }
    void setOperand3(Operand *o) { src3 = o; }
    
    InstrType getType() { return type; }
    Type *getDataType() { return dataType; }
    Operand *getDest() { return dest; }
    Operand *getOperand1() { return src1; }
    Operand *getOperand2() { return src2; }
    Operand *getOperand3() { return src3; }
    
    void print();
protected:
    Type *dataType;
    InstrType type = InstrType::None;
    Operand *dest = nullptr;
    Operand *src1 = nullptr;
    Operand *src2 = nullptr;
    Operand *src3 = nullptr;
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
    explicit Imm(uint64_t imm) : Operand(OpType::Imm) {
        this->imm = imm;
    }
    
    uint64_t getValue() { return imm; }
    
    void print();
private:
    uint64_t imm = 0;
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
    
    void print();
private:
    std::string name = "";
};

} // end namespace LLIR

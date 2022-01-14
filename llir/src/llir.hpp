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
    
    // Bitwise operations
    And,
    Or,
    Xor,
    
    // Comparisons
    ICmpEQ,
    ICmpNE,
    ICmpSGT,
    ICmpSLT,
    ICmpSGE,
    ICmpSLE,
    
    // Jumps
    // We're going to use RISC-V style because these will be the easiest
    // to translate on different architectures
    Br,
    Bc,
    
    // Function calls
    Call,
    
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
    Label,
    String,
    
    Mem,
    HReg
};

// Forward declarations
class Function;
class Block;
class Instruction;
class Operand;
class StringPtr;

//
// The base of all LLIR projects
//
class Module {
public:
    explicit Module(std::string name) {
        this->name = name;
    }
    
    void addFunction(Function *func) { functions.push_back(func); }
    void addStringPtr(StringPtr *ptr) { strings.push_back(ptr); }
    
    std::string getName() { return name; }
    int getFunctionCount() { return functions.size(); }
    Function *getFunction(int pos) { return functions.at(pos); }
    int getStringCount() { return strings.size(); }
    StringPtr *getString(int pos) { return strings.at(pos); }
    
    void transform();
    
    void print();
private:
    std::string name = "";
    std::vector<Function *> functions;
    std::vector<StringPtr *> strings;
};

//
// Represents a data type
//
class Type {
public:
    explicit Type(DataType type) {
        this->type = type;
    }
    
    // Convience creators for types
    static Type *createVoidType() { return new Type(DataType::Void); }
    static Type *createI8Type() { return new Type(DataType::I8); }
    static Type *createI16Type() { return new Type(DataType::I16); }
    static Type *createI32Type() { return new Type(DataType::I32); }
    static Type *createI64Type() { return new Type(DataType::I64); }
    
    DataType getType() { return type; }
    
    void print();
protected:
    DataType type = DataType::Void;
};

//
// Represents a basic block
//
class Block {
public:
    explicit Block(std::string name) {
        this->name = name;
    }
    
    void addInstruction(Instruction *i) { instrs.push_back(i); }
    void setPosition(int pos) { this->pos = pos; }
    
    std::string getName() { return name; }
    int getPosition() { return pos; }
    
    int getInstrCount() { return instrs.size(); }
    Instruction *getInstruction(int pos) { return instrs.at(pos); }
    
    void print();
private:
    std::string name = "";
    std::vector<Instruction *> instrs;
    int pos = 0;
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
    
    static Function *Create(std::string name, Linkage linkage, Type *dataType) {
        Function *func = new Function(name, linkage);
        func->setDataType(dataType);
        return func;
    }
    
    void setDataType(Type *d) {
        if (dataType) delete dataType;
        dataType = d;
    }
    
    void addBlock(Block *block) {
        blocks.push_back(block);
        block->setPosition(blocks.size() - 1);
    }
    
    void addBlockAfter(Block *block, Block *newBlock) {
        int pos = block->getPosition();
        blocks.insert(blocks.begin() + pos + 1, newBlock);
        newBlock->setPosition(pos + 1);
    }
    
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
    
    virtual void print();
protected:
    Type *dataType;
    InstrType type = InstrType::None;
    Operand *dest = nullptr;
    Operand *src1 = nullptr;
    Operand *src2 = nullptr;
    Operand *src3 = nullptr;
};

// Represents function calls
class FunctionCall : public Instruction {
public:
    explicit FunctionCall(std::string name, std::vector<Operand *> args) : Instruction(InstrType::Call) {
        this->name = name;
        this->args = args;
    }
    
    void setArgs(std::vector<Operand *> args) { this->args = args; }
    
    std::string getName() { return name; }
    std::vector<Operand *> getArgs() { return args; }
    
    void print();
private:
    std::string name = "";
    std::vector<Operand *> args;
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

} // end namespace LLIR

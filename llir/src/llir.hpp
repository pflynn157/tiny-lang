#pragma once

#include <string>
#include <vector>

#include <llir_operand.hpp>

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
    Ptr,
    Struct
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
    Not,
    
    // Jumps
    // We're going to use RISC-V style because these will be the easiest
    // to translate on different architectures
    Br,
    Beq,
    Bne,
    Bgt,
    Blt,
    Bge,
    Ble,
    
    // Function calls
    Call,
    
    // Memory
    Alloca,
    StructLoad,
    Load,
    GEP,
    StructStore,
    Store
};

// Forward declarations
class Function;
class Block;
class Instruction;
class Operand;
class Reg;
class StringPtr;

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
    
    virtual void print();
protected:
    explicit Type() {}
    DataType type = DataType::Void;
};

class PointerType : public Type {
public:
    explicit PointerType(Type *baseType) {
        this->type = DataType::Ptr;
        this->baseType = baseType;
    }
    
    explicit PointerType(DataType type) {
        this->type = DataType::Ptr;
        this->baseType = new Type(type);
    }
    
    static PointerType *createVoidPtrType() { return new PointerType(DataType::Void); }
    static PointerType *createI8PtrType() { return new PointerType(DataType::I8); }
    static PointerType *createI16PtrType() { return new PointerType(DataType::I16); }
    static PointerType *createI32PtrType() { return new PointerType(DataType::I32); }
    static PointerType *createI64PtrType() { return new PointerType(DataType::I64); }
    
    Type *getBaseType() { return baseType; }
    
    void print();
private:
    Type *baseType = nullptr;
};

class StructType : public Type {
public:
    explicit StructType(std::string name, std::vector<Type *> elementTypes) {
        this->type = DataType::Struct;
        this->name = name;
        this->elementTypes = elementTypes;
    }
    
    std::vector<Type *> getElementTypes() { return elementTypes; }
    
    void print();
private:
    std::string name = "";
    std::vector<Type *> elementTypes;
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
    void setID(int id) { this->id = id; }
    
    std::string getName() { return name; }
    int getID() { return id; }
    
    int getInstrCount() { return instrs.size(); }
    Instruction *getInstruction(int pos) { return instrs.at(pos); }
    
    void print();
private:
    std::string name = "";
    std::vector<Instruction *> instrs;
    int id = 0;
};

//
// Represents a function
//
// TODO: We can probably get rid of stack
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
    
    void setArgs(std::vector<Type *> args) {
        this->args = args;
        for (int i = 0; i<args.size(); i++) {
            Reg *reg = new Reg(std::to_string(i));
            varRegs.push_back(reg);
        }
    }
    
    void addBlock(Block *block) {
        block->setID(blockID);
        ++blockID;
        blocks.push_back(block);
    }
    
    void addBlockAfter(Block *block, Block *newBlock) {
        for (int i = 0; i<blocks.size(); i++) {
            if (block->getID() == blocks.at(i)->getID()) {
                blocks.insert(blocks.begin() + i + 1, newBlock);
                newBlock->setID(blockID);
                ++blockID;
            }
        }
    }
    
    void setStackSize(int size) { stackSize = size; }
    
    std::string getName() { return name; }
    Linkage getLinkage() { return linkage; }
    int getStackSize() { return stackSize; }
    
    int getBlockCount() { return blocks.size(); }
    Block *getBlock(int pos) { return blocks.at(pos); }
    
    int getArgCount() { return args.size(); }
    Type *getArgType(int pos) { return args.at(pos); }
    Reg *getArg(int pos) { return varRegs.at(pos); }
    
    void print();
private:
    Type *dataType;
    std::string name = "";
    Linkage linkage = Linkage::Local;
    std::vector<Block *> blocks;
    std::vector<Type *> args;
    std::vector<Reg *> varRegs;
    int stackSize = 0;
    int blockID = 1;
};

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
    
    Function *getFunctionByName(std::string fname) {
        for (Function *f : functions) {
            if (f->getName() == fname) return f;
        }
        return nullptr;
    }
    
    void transform();
    
    void print();
private:
    std::string name = "";
    std::vector<Function *> functions;
    std::vector<StringPtr *> strings;
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

} // end namespace LLIR

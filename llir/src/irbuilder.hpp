#pragma once

#include <llir.hpp>

namespace LLIR {

class IRBuilder {
public:
    explicit IRBuilder(Module *mod);

    void setCurrentFunction(Function *func) { currentFunc = func; };
    
    // Creates a new block and sets the insert point
    Block *createBlock(std::string name);
    void addBlock(Block *block);
    void addBlockAfter(Block *block, Block *newBlock);
    void setInsertPoint(Block *block);
    Block *getInsertPoint();
    
    //
    // Operand builders
    //
    Operand *createI8(int8_t val);
    Operand *createI16(int16_t val);
    Operand *createI32(int val);
    Operand *createI64(int64_t val);
    Operand *createString(std::string val);
    
    //
    // Instruction builders
    //
    Reg *createAlloca(Type *type);
    Instruction *createStore(Type *type, Operand *op, Operand *dest);
    Reg *createLoad(Type *type, Operand *src);
    Operand *createAdd(Type *type, Operand *op1, Operand *op2);
    Operand *createSub(Type *type, Operand *op1, Operand *op2);
    Operand *createSMul(Type *type, Operand *op1, Operand *op2);
    Operand *createSDiv(Type *type, Operand *op1, Operand *op2);
    Operand *createAnd(Type *type, Operand *op1, Operand *op2);
    Operand *createOr(Type *type, Operand *op1, Operand *op2);
    Operand *createXor(Type *type, Operand *op1, Operand *op2);
    Operand *createNeg(Type *type, Operand *op1);
    Reg *createBeq(Type *type, Operand *op1, Operand *op2, Block *destBlock);
    Instruction *createBr(Block *block);
    Instruction *createVoidCall(std::string name, std::vector<Operand *> args);
    Instruction *createRetVoid();
    Instruction *createRet(Type *type, Operand *op);
protected:
    Operand *createBinaryOp(Type *type, Operand *op1, Operand *op2, InstrType iType);
private:
    Module *mod;
    Function *currentFunc;
    Block *currentBlock;
    int regCounter = 0;
    int lblCounter = 0;
};

}

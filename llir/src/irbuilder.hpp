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
    Operand *createI32(int val);
    Operand *createString(std::string val);
    
    //
    // Instruction builders
    //
    Reg *createAlloca(Type *type);
    Instruction *createStore(Type *type, Operand *op, Operand *dest);
    Reg *createLoad(Type *type, Operand *src);
    Reg *createAdd(Type *type, Operand *op1, Operand *op2);
    Reg *createSub(Type *type, Operand *op1, Operand *op2);
    Reg *createAnd(Type *type, Operand *op1, Operand *op2);
    Reg *createOr(Type *type, Operand *op1, Operand *op2);
    Reg *createXor(Type *type, Operand *op1, Operand *op2);
    Reg *createBeq(Type *type, Operand *op1, Operand *op2, Block *destBlock);
    Instruction *createBr(Block *block);
    Instruction *createVoidCall(std::string name, std::vector<Operand *> args);
    Instruction *createRetVoid();
    Instruction *createRet(Type *type, Operand *op);
private:
    Module *mod;
    Function *currentFunc;
    Block *currentBlock;
    int regCounter = 0;
    int lblCounter = 0;
};

}

#pragma once

#include <llir.hpp>

namespace LLIR {

class IRBuilder {
public:
    explicit IRBuilder(Module *mod);

    void setCurrentFunction(Function *func) { currentFunc = func; };
    
    // Creates a new block and sets the insert point
    Block *createBlock(std::string name);
    
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

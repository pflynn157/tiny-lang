#pragma once

#include <llir.hpp>

namespace LLIR {

class IRBuilder {
public:
    void setCurrentFunction(Function *func) { currentFunc = func; };
    
    // Creates a new block and sets the insert point
    Block *createBlock(std::string name);
    
    //
    // Operand builders
    //
    Operand *createI32(int val);
    
    //
    // Instruction builders
    //
    Reg *createAlloca(Type *type);
    Instruction *createRetVoid();
    Instruction *createRet(Type *type, Operand *op);
private:
    Function *currentFunc;
    Block *currentBlock;
    int regCounter = 0;
};

}

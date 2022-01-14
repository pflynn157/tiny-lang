#include <llir.hpp>
#include <irbuilder.hpp>

namespace LLIR {

Block *IRBuilder::createBlock(std::string name) {
    currentBlock = new Block(name);
    currentFunc->addBlock(currentBlock);
    return currentBlock;
}

Operand *IRBuilder::createI32(int val) {
    return new Imm(val);
}

Reg *IRBuilder::createAlloca(Type *type) {
    Instruction *alloc = new Instruction(InstrType::Alloca);
    alloc->setDataType(type);
    
    Reg *dest = new Reg(std::to_string(regCounter));
    ++regCounter;
    alloc->setDest(dest);
    
    currentBlock->addInstruction(alloc);
    return dest;
}

Instruction *IRBuilder::createStore(Type *type, Operand *op, Operand *dest) {
    Instruction *store = new Instruction(InstrType::Store);
    store->setDataType(type);
    store->setOperand1(op);
    store->setOperand2(dest);
    currentBlock->addInstruction(store);
    return store;
}

Instruction *IRBuilder::createRetVoid() {
    Instruction *ret = new Instruction(InstrType::Ret);
    ret->setDataType(Type::createVoidType());
    currentBlock->addInstruction(ret);
    return ret;
}

Instruction *IRBuilder::createRet(Type *type, Operand *op) {
    Instruction *ret = new Instruction(InstrType::Ret);
    ret->setDataType(type);
    ret->setOperand1(op);
    currentBlock->addInstruction(ret);
    return ret;
}

} // end namespace LLIR

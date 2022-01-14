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
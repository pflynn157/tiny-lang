#include <llir.hpp>
#include <irbuilder.hpp>

namespace LLIR {

IRBuilder::IRBuilder(Module *mod) {
    this->mod = mod;
}

Block *IRBuilder::createBlock(std::string name) {
    currentBlock = new Block(name);
    currentFunc->addBlock(currentBlock);
    return currentBlock;
}

void IRBuilder::addBlock(Block *block) {
    currentFunc->addBlock(block);
}

void IRBuilder::addBlockAfter(Block *block, Block *newBlock) {
    currentFunc->addBlockAfter(block, newBlock);
}

void IRBuilder::setInsertPoint(Block *block) {
    currentBlock = block;
}

Block *IRBuilder::getInsertPoint() {
    return currentBlock;
}

Operand *IRBuilder::createI8(int8_t val) {
    return new Imm(val);
}

Operand *IRBuilder::createI16(int16_t val) {
    return new Imm(val);
}

Operand *IRBuilder::createI32(int val) {
    return new Imm(val);
}

Operand *IRBuilder::createI64(int64_t val) {
    return new Imm(val);
}

Operand *IRBuilder::createString(std::string val) {
    std::string name = "STR" + std::to_string(lblCounter);
    ++lblCounter;
    
    StringPtr *ptr = new StringPtr(name, val);
    mod->addStringPtr(ptr);
    
    return ptr;
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

Reg *IRBuilder::createLoad(Type *type, Operand *src) {
    Instruction *load = new Instruction(InstrType::Load);
    load->setDataType(type);
    load->setOperand1(src);
    
    Reg *dest = new Reg(std::to_string(regCounter));
    ++regCounter;
    load->setDest(dest);
    
    currentBlock->addInstruction(load);
    return dest;
}

Reg *IRBuilder::createAdd(Type *type, Operand *op1, Operand *op2) {
    Instruction *add = new Instruction(InstrType::Add);
    add->setDataType(type);
    add->setOperand1(op1);
    add->setOperand2(op2);
    
    Reg *dest = new Reg(std::to_string(regCounter));
    ++regCounter;
    add->setDest(dest);
    
    currentBlock->addInstruction(add);
    return dest;
}

Reg *IRBuilder::createSub(Type *type, Operand *op1, Operand *op2) {
    Instruction *sub = new Instruction(InstrType::Sub);
    sub->setDataType(type);
    sub->setOperand1(op1);
    sub->setOperand2(op2);
    
    Reg *dest = new Reg(std::to_string(regCounter));
    ++regCounter;
    sub->setDest(dest);
    
    currentBlock->addInstruction(sub);
    return dest;
}

Reg *IRBuilder::createSMul(Type *type, Operand *op1, Operand *op2) {
    Instruction *op = new Instruction(InstrType::SMul);
    op->setDataType(type);
    op->setOperand1(op1);
    op->setOperand2(op2);
    
    Reg *dest = new Reg(std::to_string(regCounter));
    ++regCounter;
    op->setDest(dest);
    
    currentBlock->addInstruction(op);
    return dest;
}

Reg *IRBuilder::createSDiv(Type *type, Operand *op1, Operand *op2) {
    Instruction *op = new Instruction(InstrType::SDiv);
    op->setDataType(type);
    op->setOperand1(op1);
    op->setOperand2(op2);
    
    Reg *dest = new Reg(std::to_string(regCounter));
    ++regCounter;
    op->setDest(dest);
    
    currentBlock->addInstruction(op);
    return dest;
}

Reg *IRBuilder::createAnd(Type *type, Operand *op1, Operand *op2) {
    Instruction *op = new Instruction(InstrType::And);
    op->setDataType(type);
    op->setOperand1(op1);
    op->setOperand2(op2);
    
    Reg *dest = new Reg(std::to_string(regCounter));
    ++regCounter;
    op->setDest(dest);
    
    currentBlock->addInstruction(op);
    return dest;
}

Reg *IRBuilder::createOr(Type *type, Operand *op1, Operand *op2) {
    Instruction *op = new Instruction(InstrType::Or);
    op->setDataType(type);
    op->setOperand1(op1);
    op->setOperand2(op2);
    
    Reg *dest = new Reg(std::to_string(regCounter));
    ++regCounter;
    op->setDest(dest);
    
    currentBlock->addInstruction(op);
    return dest;
}

Reg *IRBuilder::createXor(Type *type, Operand *op1, Operand *op2) {
    Instruction *op = new Instruction(InstrType::Xor);
    op->setDataType(type);
    op->setOperand1(op1);
    op->setOperand2(op2);
    
    Reg *dest = new Reg(std::to_string(regCounter));
    ++regCounter;
    op->setDest(dest);
    
    currentBlock->addInstruction(op);
    return dest;
}

Reg *IRBuilder::createBeq(Type *type, Operand *op1, Operand *op2, Block *destBlock) {
    Instruction *op = new Instruction(InstrType::Beq);
    op->setDataType(type);
    op->setOperand1(op1);
    op->setOperand2(op2);
    op->setOperand3(new Label(destBlock->getName()));
    
    Reg *dest = new Reg(std::to_string(regCounter));
    ++regCounter;
    op->setDest(dest);
    
    currentBlock->addInstruction(op);
    return dest;
}

Instruction *IRBuilder::createBr(Block *block) {
    Label *lbl = new Label(block->getName());
    Instruction *op = new Instruction(InstrType::Br);
    op->setOperand1(lbl);
    currentBlock->addInstruction(op);
    return op;
}

Instruction *IRBuilder::createVoidCall(std::string name, std::vector<Operand *> args) {
    FunctionCall *fc = new FunctionCall(name, args);
    currentBlock->addInstruction(fc);
    return fc;
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

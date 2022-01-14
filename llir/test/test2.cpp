#include <iostream>

#include <llir.hpp>
#include <amd64.hpp>
using namespace LLIR;

int main(int argc, char **argv) {
    Module *mod = new Module("mod1");
    
    //
    // func main:
    //     %1 = alloca i32
    //     store i32 10, %1
    //     %2 = load i32 %1
    //     ret 5
    Function *mainFunc = new Function("main", Linkage::Global);
    mainFunc->setDataType(new Type(DataType::I32));
    mainFunc->setStackSize(16);
    mod->addFunction(mainFunc);
    Block *b1 = new Block("entry");
    mainFunc->addBlock(b1);
    
    Instruction *alloc1 = new Instruction(InstrType::Alloca);
    alloc1->setDataType(new Type(DataType::I32));
    alloc1->setDest(new Reg("1"));
    b1->addInstruction(alloc1);
    
    Instruction *store = new Instruction(InstrType::Store);
    store->setDataType(new Type(DataType::I32));
    store->setOperand1(new Imm(10));
    store->setOperand2(new Reg("1"));
    b1->addInstruction(store);
    
    Instruction *load = new Instruction(InstrType::Load);
    load->setDataType(new Type(DataType::I32));
    load->setDest(new Reg("2"));
    load->setOperand1(new Reg("1"));
    b1->addInstruction(load);
    
    Instruction *ret = new Instruction(InstrType::Ret);
    ret->setDataType(new Type(DataType::I32));
    ret->setOperand1(new Reg("2"));
    b1->addInstruction(ret);
    
    mod->print();
    std::cout << "---------------" << std::endl;
    Amd64Writer *writer = new Amd64Writer(mod);
    writer->compile();
    writer->dump();
    writer->writeToFile();
    writer->build();
    return 0;
}
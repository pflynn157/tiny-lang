#include <iostream>

#include <llir.hpp>
using namespace LLIR;

int main(int argc, char **argv) {
    Module *mod = new Module("mod1");
    
    //
    // func main:
    //     ret 5
    Function *mainFunc = new Function("main", Linkage::Global);
    mainFunc->setDataType(new Type(DataType::I32));
    mod->addFunction(mainFunc);
    Block *b1 = new Block("entry");
    mainFunc->addBlock(b1);
    
    Instruction *ret = new Instruction(InstrType::Ret);
    ret->setDataType(new Type(DataType::I32));
    ret->setOperand1(new Imm(5));
    b1->addInstruction(ret);
    
    mod->print();
    return 0;
}
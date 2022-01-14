#include <iostream>

#include <x86ir.hpp>
using namespace LLIR;

int main(int argc, char **argv) {
    X86File *file = new X86File("first");
    X86GlobalFunc *mainFunc = new X86GlobalFunc("main");
    file->addCode(mainFunc);
    
    X86Push *p1 = new X86Push(new X86Reg64(X86Reg::BP));
    file->addCode(p1);

    std::cout << file->print() << std::endl;
    return 0;
}
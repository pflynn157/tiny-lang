#pragma once

#include <string>
#include <map>

#include <llir.hpp>
#include <x86ir.hpp>

namespace LLIR {

class Amd64Writer {
public:
    explicit Amd64Writer(Module *mod);
    void compile();
    void compileInstruction(Instruction *instr);
    X86Operand *compileOperand(Operand *src, Type *type);
    void dump();
    void writeToFile();
    void writeToFile(std::string path);
    void build();
protected:
    std::string getSizeForType(Type *type);
private:
    Module *mod = nullptr;
    X86File *file;
    
    int stackPos = 0;
    std::map<std::string, int> memMap;
    std::map<int, X86Reg> regMap;
};

} // end namespace LLIR
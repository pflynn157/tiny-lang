#include <map>
#include <vector>
#include <string>

#include <llir.hpp>

namespace LLIR {

// By default, all operands in LLIR are virtual registers, which are naturally not
// suitable to hardware transformation
//
// This pass translates all virtual registers to hardware registers and memory operands
// as appropriate.
//
// The algorithm is very simple. Basically, we look for "allocas", and assign memory operands to
// the destination, and replace everywhere else.
//
void Module::transform() {
    for (Function *func : functions) {
        // The maps
        std::vector<std::string> memList;
        std::map<std::string, int> regMap;
        int regCount = 0;
        
        for (int i = 0; i<func->getBlockCount(); i++) {
            Block *block = func->getBlock(i);
            for (int j = 0; j<block->getInstrCount(); j++) {
                Instruction *instr = block->getInstruction(j);
                if (instr->getType() == InstrType::Alloca) {
                    Reg *reg = static_cast<Reg *>(instr->getDest());
                    memList.push_back(reg->getName());
                    
                    Mem *mem = new Mem(reg->getName());
                    instr->setDest(mem);
                    continue;
                }
                
                // Otherwise, check and switch the operands
            }
        }
    }
}

} // end LLIR

#include <iostream>

#include <preproc/Preproc.hpp>
#include <parser/Parser.hpp>
#include <ast/ast.hpp>

int main(int argc, char **argv) {
    if (argc == 1) {
        std::cerr << "Error: No input file." << std::endl;
        return 1;
    }
    
    // Other flags
    std::string input = "";
    bool printAst = false;
    
    for (int i = 1; i<argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "--ast") {
            printAst = true;
        } else if (arg[0] == '-') {
            std::cerr << "Invalid option: " << arg << std::endl;
            return 1;
        } else {
            input = arg;
        }
    }
    
    //
    // Build the AST
    //
    std::string newInput = preprocessFile(input, false);
    if (newInput == "") {
        return 1;
    }
    
    Parser *frontend = new Parser(newInput);
    AstTree *tree;
    if (!frontend->parse()) {
        delete frontend;
        return 1;
    }
    
    tree = frontend->getTree();
    
    delete frontend;
    remove(newInput.c_str());
    
    if (printAst) {
        tree->print();
        return 0;
    }
    
    //
    // If we make it here, run the VM
    //
    
    return 0;
}


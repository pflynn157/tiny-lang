cmake_minimum_required(VERSION 3.0.0)
project(tl_frontend)

set(SRC
    # Frontend sources
    lex/Lex.cpp
    
    debug/AstDebug.cpp
    debug/LexDebug.cpp
    debug/Unparse.cpp
    
    parser/ErrorManager.cpp
    parser/Expression.cpp
    parser/Flow.cpp
    parser/Function.cpp
    parser/Parser.cpp
    parser/Structure.cpp
    parser/Variable.cpp
    
    preproc/Preproc.cpp
    
    # Compile sources
    compiler/Builder.cpp
    compiler/Compiler.cpp
    compiler/Flow.cpp
    compiler/Function.cpp
    compiler/Variable.cpp
    
    # Program sources
    main.cpp
)

add_executable(tlc ${SRC})

llvm_map_components_to_libnames(llvm_libs support core irreader target asmparser
    X86AsmParser
    X86CodeGen
    X86Info
)

target_link_libraries(tlc
    ${llvm_libs}
)


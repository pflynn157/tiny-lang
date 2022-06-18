cmake_minimum_required(VERSION 3.0.0)
project(tl_frontend)

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/src/lex.cpp
           ${CMAKE_BINARY_DIR}/src/lex.hpp
           ${CMAKE_BINARY_DIR}/src/lex_debug.cpp
    COMMAND minilex ${CMAKE_SOURCE_DIR}/src/lex/lex
    DEPENDS ${CMAKE_SOURCE_DIR}/src/lex/lex.py
)
include_directories(${CMAKE_BINARY_DIR}/src)

if (LLIR)
    set(COMPILER_SRC
    
    )
else()
    set(COMPILER_SRC
        compiler/LLVM/Builder.cpp
        compiler/LLVM/Compiler.cpp
        compiler/LLVM/Flow.cpp
        compiler/LLVM/Function.cpp
        compiler/LLVM/Variable.cpp
    )
endif()

set(SRC
    # Frontend sources
    ${CMAKE_BINARY_DIR}/src/lex.cpp
    ${CMAKE_BINARY_DIR}/src/lex_debug.cpp
    
    debug/AstDebug.cpp
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
    ${COMPILER_SRC}
    
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


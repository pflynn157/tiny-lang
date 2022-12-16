cmake_minimum_required(VERSION 3.0.0)
project(tl_frontend)

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/src/lex.cpp
           ${CMAKE_BINARY_DIR}/src/lex.hpp
           ${CMAKE_BINARY_DIR}/src/lex_debug.cpp
    COMMAND minilex ${CMAKE_SOURCE_DIR}/src/lex/lex
    DEPENDS ${CMAKE_SOURCE_DIR}/src/lex/lex.py
)

set(SRC
    # Frontend sources
    ${CMAKE_BINARY_DIR}/src/lex.cpp
    ${CMAKE_BINARY_DIR}/src/lex_debug.cpp
    
    ast/ast.cpp
    ast/ast_builder.cpp
    ast/AstDebug.cpp
    ast/astdot.cpp
    
    parser/ErrorManager.cpp
    parser/Expression.cpp
    parser/Flow.cpp
    parser/Function.cpp
    parser/Parser.cpp
    parser/Structure.cpp
    parser/Variable.cpp
    
    preproc/Preproc.cpp
)

add_library(tlcparser ${SRC})


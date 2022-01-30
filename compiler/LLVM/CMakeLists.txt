cmake_minimum_required(VERSION 3.0.0)
project(tl_compiler_llvm)

set(SRC
    Builder.cpp
    Compiler.cpp
    Flow.cpp
    Function.cpp
    Variable.cpp
)

add_library(tlcompiler_llvm STATIC ${SRC})

llvm_map_components_to_libnames(llvm_libs support core irreader target asmparser
    X86AsmParser
    X86CodeGen
    X86Info
)

target_link_libraries(tlcompiler_llvm ${llvm_libs})



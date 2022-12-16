cmake_minimum_required(VERSION 3.0.0)
project(tl_llvm)

set(COMPILER_SRC
    Builder.cpp
    Compiler.cpp
    Flow.cpp
    Function.cpp
    Variable.cpp
)

add_executable(tlc ${COMPILER_SRC} main.cpp)

llvm_map_components_to_libnames(llvm_libs support core irreader target asmparser
    X86AsmParser
    X86CodeGen
    X86Info
)

target_link_libraries(tlc
    tlcparser
    ${llvm_libs}
)


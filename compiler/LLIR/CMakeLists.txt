cmake_minimum_required(VERSION 3.0.0)
project(tl_compiler_llir)

set(SRC
    Builder.cpp
    Compiler.cpp
    Flow.cpp
    Function.cpp
)

add_library(tlcompiler_llir STATIC ${SRC})

target_link_libraries(tlcompiler_llir llir)



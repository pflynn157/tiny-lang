cmake_minimum_required(VERSION 3.0.0)
project(tiny_lang)

find_package(LLVM REQUIRED CONFIG)

add_compile_options(-std=c++17 -g)

include_directories(src)

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS})

add_subdirectory(src)
add_subdirectory(lib)


cmake_minimum_required(VERSION 3.0.0)
project(orka_main)

set(SRC
    main.cpp
)

add_executable(occ ${SRC})
add_executable(llir llir_main.cpp)

target_link_libraries(occ
    ocparser
    occompiler_llvm
)

target_link_libraries(llir
    ocparser
    occompiler_llir
)
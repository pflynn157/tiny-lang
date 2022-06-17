cmake_minimum_required(VERSION 3.0.0)
project(tl_main)

set(SRC
    main.cpp
)

add_executable(tlc ${SRC})

target_link_libraries(tlc
    tlparser
    tlcompiler_llvm
)


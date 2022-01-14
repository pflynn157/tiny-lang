cmake_minimum_required(VERSION 3.0.0)
project(llir_src)

set(SRC
    amd64.cpp
    irbuilder.cpp
    print.cpp
    transform.cpp
    x86ir.cpp
)

add_library(llir SHARED ${SRC})

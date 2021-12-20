cmake_minimum_required(VERSION 3.0.0)
project(orka_clib)

set(SRC
    io.cpp
    mem.cpp
    strutils.cpp
)

add_library(orka_clib SHARED ${SRC})
add_library(orka_clib2 SHARED ${SRC})

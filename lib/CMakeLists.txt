cmake_minimum_required(VERSION 3.0.0)
project(tinylang_lib)

set(SRC
    io.cpp
    str.cpp
)

add_library(tinylang SHARED ${SRC})

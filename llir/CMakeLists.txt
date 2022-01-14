cmake_minimum_required(VERSION 3.0.0)
project(llir)

add_compile_options(-std=c++14)

include_directories(src)

add_subdirectory(src)
add_subdirectory(test)

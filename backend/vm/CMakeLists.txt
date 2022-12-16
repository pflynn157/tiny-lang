cmake_minimum_required(VERSION 3.0.0)
project(tl_vm)

set(SRC
    main.cpp
)

add_executable(tlvm ${SRC})

target_link_libraries(tlvm tlcparser)


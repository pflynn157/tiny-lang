cmake_minimum_required(VERSION 3.0.0)
project(orka)

find_package(LLVM REQUIRED CONFIG)

add_compile_options(-std=c++17 -g)

option(FEDORA "On Fedora OS" OFF)
if (FEDORA)
    add_definitions(-DLINK_FEDORA)
endif()

option(BUILD_GUI "Build GUI components")

include_directories(frontend compiler/LLIR compiler/LLVM)
include_directories(ide/src ide/lib)

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS})

add_subdirectory(frontend)
add_subdirectory(compiler)
add_subdirectory(src)
add_subdirectory(lib)

if (BUILD_GUI)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC ON)
    
    find_package(Qt5 REQUIRED COMPONENTS
        Core
        Widgets
        Gui
        Xml
    )
    
    add_subdirectory(ide)
endif()

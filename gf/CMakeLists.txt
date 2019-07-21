cmake_minimum_required(VERSION 3.0)
cmake_policy(SET CMP0048 NEW)

project(gf VERSION 0.1.0 LANGUAGES CXX)

add_definitions(-DNOMINMAX)

add_executable(gf
    FileNode.cpp
    FileNode.h
    FileTree.cpp
    FileTree.h
    main.cpp
)

if(MSVC)
    target_compile_options(gf PRIVATE "/std:c++17")
endif()

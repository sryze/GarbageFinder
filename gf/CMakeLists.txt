cmake_minimum_required(VERSION 3.0)
cmake_policy(SET CMP0048 NEW)

project(gf VERSION 0.1.0 LANGUAGES CXX)

add_executable(gf
    FileNode.cpp
    FileNode.h
    FileTree.cpp
    FileTree.h
    main.cpp
)

if(UNIX)
    target_compile_options(gf PRIVATE "-std=c++17")
endif()

iF(WIN32)
    target_compile_definitions(gf PRIVATE -DNOMINMAX -DWIN32_LEAN_AND_MEAN)
endif()

if(MSVC)
    target_compile_options(gf PRIVATE "/std:c++17")
endif()

cmake_minimum_required(VERSION 3.0)
cmake_policy(SET CMP0048 NEW)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(GarbageFinder VERSION 0.1.0 LANGUAGES CXX)

add_executable(GarbageFinder
    FileNode.cpp
    FileNode.h
    FileSystem.cpp
    FileSystem.h
    Error.cpp
    Error.h
    main.cpp
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR
   CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

iF(WIN32)
    target_compile_definitions(GarbageFinder PRIVATE -DNOMINMAX -DWIN32_LEAN_AND_MEAN)
endif()

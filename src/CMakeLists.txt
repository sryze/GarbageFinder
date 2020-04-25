cmake_minimum_required(VERSION 3.8)
cmake_policy(SET CMP0048 NEW)

project(gf VERSION 0.1.0 LANGUAGES CXX)

add_executable(gf
    FileNode.cpp
    FileNode.h
    FileSystem.cpp
    FileSystem.h
    Error.cpp
    Error.h
    main.cpp
)

target_compile_features(gf PRIVATE cxx_std_11)

iF(WIN32)
    target_compile_definitions(gf PRIVATE -DNOMINMAX -DWIN32_LEAN_AND_MEAN)
endif()

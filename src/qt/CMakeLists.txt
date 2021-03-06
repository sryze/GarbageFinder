cmake_minimum_required(VERSION 3.0)
cmake_policy(SET CMP0048 NEW)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(GarbageFinderQt VERSION 0.1.0 LANGUAGES CXX)

list(APPEND
     CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR
   CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC OFF)

set(HEADERS
  mainwindow.h
)

set(SOURCES
  main.cpp
  mainwindow.cpp
)

add_executable(GarbageFinderQt
  ${SOURCES}
  ${HEADERS}
  ${EMBEDDED_RESOURCES}
  ${RESORUCES}
)

target_link_libraries(GarbageFinderQt
  Qt5::Core
  Qt5::Widgets
)

if(WIN32)
  if(MSVC)
    set_property(TARGET GarbageFinderQt APPEND_STRING PROPERTY
                 LINK_FLAGS " /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
  elseif(MINGW)
    set_property(TARGET GarbageFinderQt APPEND_STRING PROPERTY
                 LINK_FLAGS " -Wl,-subsystem,windows")
  endif()
endif()
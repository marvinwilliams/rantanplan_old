cmake_minimum_required (VERSION 3.14)
project (rantanplan)
set (rantanplan_VERSION_MAJOR 0)
set (rantanplan_VERSION_MINOR 1)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_FLAGS "-std=c++17")

add_compile_options (
  -Wall
  -Wextra
  -Wshadow
  -Wnon-virtual-dtor
  -Wcast-align
  -Wunused
  -Woverloaded-virtual
  -Wpedantic
  -Wconversion
  -Wsign-conversion
  -Wnull-dereference
  -Weffc++
  )

include_directories ("${PROJECT_SOURCE_DIR}/src")

configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/conig.h"
  )

include_directories ("${PROJECT_BINARY_DIR}")

add_custom_command (
  OUTPUT ${PROJECT_SOURCE_DIR}/parser/parser.cxx
  COMMAND bison -o ${PROJECT_SOURCE_DIR}/parser/parser.cxx

add_executable(rantanplan rantanplan.cpp)

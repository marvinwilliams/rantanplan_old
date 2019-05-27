cmake_minimum_required(VERSION 3.9)
project(rantanplan)
set(rantanplan_VERSION_MAJOR 0)
set(rantanplan_VERSION_MINOR 1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-std=c++17")

add_compile_options(
  -Wall
  -Wextra
  #-Wshadow
  -Wnon-virtual-dtor
  -Wcast-align
  -Wunused
  -Woverloaded-virtual
  -Wpedantic
  -Wconversion
  -Wsign-conversion
  -Wnull-dereference
  )

add_subdirectory(src)

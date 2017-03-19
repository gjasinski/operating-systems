cmake_minimum_required(VERSION 3.6)
set(CMAKE_C_COMPILER "/usr/bin/gcc")
set(CMAKE_CXX_COMPILER "/usr/bin/g++")
project(zad1)



set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c)
add_executable(zad1 ${SOURCE_FILES})
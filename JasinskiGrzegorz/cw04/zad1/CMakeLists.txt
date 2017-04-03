cmake_minimum_required(VERSION 3.0)
project(zad1)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -std=c99")

set(SOURCE_FILES main.c)
add_executable(zad1 ${SOURCE_FILES})

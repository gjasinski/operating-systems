cmake_minimum_required(VERSION 3.0)
project(untitled)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -pthread")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -std=c99 -pthread")

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c)
add_executable(untitled ${SOURCE_FILES})

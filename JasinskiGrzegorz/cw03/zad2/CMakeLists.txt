cmake_minimum_required(VERSION 3.0)
project(zad2)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -std=c99")

set(SOURCE_FILES main.c)
add_executable(zad2 ${SOURCE_FILES})

set(DEMO_MEMORY_SOURCE_FILES demo_memory.c)
add_executable(demo_memory ${DEMO_MEMORY_SOURCE_FILES})

set(DEMO_TIME_SOURCE_FILES demo_time.c)
add_executable(demo_time ${DEMO_TIME_SOURCE_FILES})

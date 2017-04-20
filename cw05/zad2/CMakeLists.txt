cmake_minimum_required(VERSION 3.0)
project(zad2)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -std=c99 -lm")

set(SOURCE_FILES main.c)
add_executable(master ${SOURCE_FILES})

set(SOURCE_FILES_CHILD child.c)
add_executable(slave ${SOURCE_FILES_CHILD})

target_link_libraries(slave m)

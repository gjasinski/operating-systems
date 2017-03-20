cmake_minimum_required (VERSION 3.0)
project(zad2)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -std=c99")

set(SOURCE_FILES addressbook.c addressbook.h bsttree.c bsttree.h list.c list.h datagenerator.h datagenerator.c main.c)
add_executable(main ${SOURCE_FILES})

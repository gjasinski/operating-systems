cmake_minimum_required (VERSION 3.0)
project(addressbook)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -std=c99")

set(SOURCE_FILES addressbook.h bsttree.c bsttree.h list.c list.h)
add_library(addressbook_static STATIC ${SOURCE_FILES})
add_library(addressbook_shared SHARED ${SOURCE_FILES})

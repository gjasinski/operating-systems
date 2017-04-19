cmake_minimum_required(VERSION 3.0)
project(zad2)

set(CMAKE_C_STANDARD 99)
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -std=c99")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}-std=c99")

set(SOURCE_FILES_SERVER server.c)
add_executable(server ${SOURCE_FILES_SERVER})

set(SOURCE_FILES_CLIENT client.c)
add_executable(client ${SOURCE_FILES_CLIENT})
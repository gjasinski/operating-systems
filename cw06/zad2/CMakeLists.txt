cmake_minimum_required(VERSION 3.0)
project(zad2)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -std=c99")

set(SOURCE_FILES server.c)
add_executable(server ${SOURCE_FILES})

set(SOURCE_FILES_CHILD client.c)
add_executable(client ${SOURCE_FILES_CHILD})

target_link_libraries(server -lrt)
target_link_libraries(client -lrt)

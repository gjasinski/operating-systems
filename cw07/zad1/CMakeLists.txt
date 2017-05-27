cmake_minimum_required(VERSION 3.0)
project(zad1)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -std=c99")

add_executable(barber barber.c)
add_executable(client_creator client_creator.c)
add_executable(client client.c)

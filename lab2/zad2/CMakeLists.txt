cmake_minimum_required(VERSION 3.0)
project(zad2)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -std=c99")

set(SOURCE_FILES1 main_open_close.c)
add_executable(main_open_close ${SOURCE_FILES1})

set(SOURCE_FILES2 main_nftw.c)
add_executable(main_nftw ${SOURCE_FILES2})

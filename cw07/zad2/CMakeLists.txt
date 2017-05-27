cmake_minimum_required(VERSION 3.0)
project(Zad2)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -std=c99 -pthread -lrt")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -pthread -lrt")

add_executable(barber barber.c)
add_executable(client_creator client_creator.c)
add_executable(client client.c)
TARGET_LINK_LIBRARIES(barber ${Boost_LIBRARIES} rt)
TARGET_LINK_LIBRARIES(client ${Boost_LIBRARIES} rt)

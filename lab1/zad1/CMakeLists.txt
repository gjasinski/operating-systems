#cmake_minimum_required (VERSION 3.6)
#project(addressbook)

#set(CMAKE_C_STANDARD 98)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -std=c99")

#add_library(AddressBookShared SHARED EXCLUDE_FROM_ALL addressbook.c addressbook.h)
#add_library(AddressBookStatic STATIC EXCLUDE_FROM_ALL addressbook.c addressbook.h)
#target_include_directories (AddressBookShared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set(SOURCE_FILES addressbook.h bsttree.c bsttree.h list.c list.h)
add_library(addressbook_static STATIC ${SOURCE_FILES})
add_library(addressbook_shared SHARED ${SOURCE_FILES})

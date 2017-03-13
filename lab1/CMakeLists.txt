cmake_minimum_required (VERSION 3.0)
#project (Address Book)

add_library(AddressBookShared SHARED addressbook.c addressbook.h)
add_library(AddressBookStatic STATIC addressbook.c addressbook.h)

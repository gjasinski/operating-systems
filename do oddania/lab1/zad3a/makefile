CC = gcc
CFLAGS = -std=c99 -Wall -pedantic -w
LINKERFLAGS=-Wl,-rpath=.

SOURCE_FILES = addressbook.c bsttree.c list.c
OBJECT_FILES = addressbook.o bsttree.o list.o

all: sharedlib staticlib

list.o: list.c list.h
	@$(CC) $(CFLAGS) -fPIC -c list.c -o list.o

bsttree.o: bsttree.c bsttree.h
		@$(CC) $(CFLAGS) -fPIC -c bsttree.c -o bsttree.o

addressbook.o: addressbook.c addressbook.h
		@$(CC) $(CFLAGS) -fPIC -c addressbook.c -o addressbook.o

sharedlib: $(OBJECT_FILES)
	$(CC) $(CFLAGS) -fPIC -shared -shared -o libad.so $(OBJECT_FILES)

staticlib: $(OBJECT_FILES)
	ar rcs libad.a $(OBJECT_FILES)

static: staticlib
	@$(CC) $(LINKERFLAGS) main.c  -o static $(CFLAGS) -static -L. -lad

dynamic: sharedlib
	@$(CC) $(LINKERFLAGS) main.c -o dynamic $(CFLAGS) -L. -lad

dll: sharedlib
	@$(CC) main.c $(CFLAGS) -o dll -ldl -DDLL

run_dynamic: dynamic
	./dynamic

run_static: static
	./static

run_dll: dll
	./dll

clean:
	@rm -f *.o
	@rm -f *.a
	@rm -f *.so
	@rm -f dynamic_main
	@rm -f static_main
	@rm -f dll_main

.PHONY: all shared static clean

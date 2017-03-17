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
	@$(CC) $(LINKERFLAGS) main.c  -o static_main $(CFLAGS) -static -L. -lad

dynamic: sharedlib
	@$(CC) $(LINKERFLAGS) main.c -o dynamic_main $(CFLAGS) -L. -lad

dll: sharedlib
	@$(CC) main.c $(CFLAGS) -o dll_main -ldl -DDLL

compile_o1: $(SOURCE_FILES)
		$(CC) -c $(CFLAGS) -O1 $^

compile_o2: $(SOURCE_FILES)
		$(CC) -c $(CFLAGS) -O2 $^

compile_o3: $(SOURCE_FILES)
		$(CC) -c $(CFLAGS) -O3 $^

run_dynamic: dynamic
	./dynamic_main

run_static: static
	./static_main

run_dll: dll
	./dll_main

clean:
	@rm -f *.o
	@rm -f *.a
	@rm -f *.so
	@rm -f dynamic_main
	@rm -f static_main
	@rm -f dll_main

.PHONY: all shared static compile_o1 compile_o2 compile_o3 clean

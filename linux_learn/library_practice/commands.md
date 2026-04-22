## after created the 2 files, stlength.c and streverse.c

make static library

gcc -c stlength.c streverse.c
ar rcs libstutils.a stlength.o streverse.o

dynamic library:
gcc -c -fPIC stlength.c streverse.c
gcc -shared stlength.o streverse.o -o libstutils2.so


Key note here:
streverse uses #import "stlength.h", but it wasnt provided to it allways

right now, you have the libraries, but they are clunky, so there must be a clean API header the users can 
use to include it, and let the OS handle the rest if its in the right path or tell the linker to do it itself

Library Internals:
stlength.c
streverse.c 
are meant to be hidden

Exposed API Header:
mylib.h
mylib2.h
is meant to be seen.

exe:

stutils.h (static one)

#ifndef STUTILS_H
#define STUTILS_H

#include "stlength.h"

int stlenth(char* str);

char* streverse(char* str);

#endif



## Next steps: organizing the library
While library is literally just the .so file, it is often packaged alongside its other code objects
(kinda like a framework on mac):

1. Public Interface (the header files, what users should include)
2. Implementation (/src, the .c and internal .h files)
3. Build Artifacts (the .so and .a files )

good library format:

mylib/
├── include/
│   └── mylib.h        ← PUBLIC API (what users include)
│
├── src/
│   ├── stlength.c
│   ├── streverse.c
│   └── internal.h     ← optional private helpers
│
├── build/
│   ├── libmylib.a
│   └── libmylib.so
│
├── Makefile
└── README.md

(this is just good developer practices, in reality):

.a and .so files go in /usr/local/lib
.h goes it /usr/local/include

On install, just create makefile rules for getting all the files where they gotta go:

install:
	mkdir -p /usr/local/include
	mkdir -p /usr/local/lib
	cp include/mylib.h /usr/local/include/
	cp libmylib.so /usr/local/lib/
	cp libmylib.a /usr/local/lib/
	ldconfig //this wil update the loader config to see the new libraries



FOR MACOS:
a littel different:

for static libraries, exactly the same, down to teh commands

for dynamic, mac uses .dylib, not .so
gcc -dynamiclib (instead fo shared), rest is exactly the same

remember to tell MACOS where it is:
DYLD_LIBRARY_PATH=.


makefiles must include:
build
clean
test (optional but very useful)
install
uninstall



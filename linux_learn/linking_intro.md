For programs like C, 3 parts:

Header File, .h, which is used during compile only!
it declares functions, used by the compiler to verify that everything is good

Static Library, .a:
contains compiled code, gets copied into your code during link-time

Dynamic Library:
contains compiled code, gets linked, but is loaded dynamically during runtime


### Full Process for Linkage:


Start with some code:

#include <stdio.h>

int main() {
    printf("Hello\n");
    return 0;
}

This is defined in some header file, stdio.h

int printf(const char *format, ...);


2: when you #include ..., it LITERALLY copies the header to the top of the file, conceptually
this tells the compiler that such a function exists, and what its intended behavior is(no linking)

3: Now onto compilation
gcc main.c -c
checks if printf is used correctly, generates the object file main.o with teh headers pre-pended
(still no idea how printf works)

4:Now, linking
gcc main.o
(didnt specify where printf lives, but still works?)
Reason: because gcc automatically links the c standard library! specifically glibc
that include /lib/libc.so /libc.a

secretly, gcc main.o => gcc main.o -lc (meaning link with libc)
(the printf is acutally implemented in libc.so somewhere, with symbol printf)

However, what about using something that is not linked by default:

#include<math.h>
double x = sqrt(5);

does not automatically link libm.so, so need to include via -lm (link math.so)

For custom libraries:

#include <foo.h>
-lfoo to load the foo dynamic/static library



How to make your own Libraries:

First, make the header (what users see)

mylib.h:

#ifndef MYLIB_H
#define MYLIB_H

void hello();

#endif


Then, implement it:

mylib.c:

#include <stdio.h>
#include "mylib.h"

void hello() {
    printf("Hello from my library");
}


Now, make the library:

Statically linked:

gcc -c mylib.c -o mylib.o

ar rcs libmylib.a mylib.o (Created static library)

(this used linux archiving tool ar to create a static library)
ar = archiver program, used to bundle object files into a single static archive (a static library)
r = replace/isert files into archive: if libmylib.a exists replace /update it
c = create archive if doesnt exist + suppresses warning
s = indexes the archive (builds symolic table so linkers can find functions faster)
libmylib.a is the output file, by convention .a on linux
libmylib.o is the compiled object file being added to the library


symbolic table:
index of everything defined in an object file or library, 
where a symbol is just the name the linker cares abt, usually:
function names (add, print_result)
or global variables
so table says:
add -> defined here
helper -> defined here

static library is just a bunch of .o files bundled together (ARCHIVE = BUNDLE FILES TOGETHER)
if left alone, linker would have to search through all the .o files for a given function, and 
see if it contains the desired symbol

instead, via ar rcs libmylib.a mylib.o, the s flag means to create a symbol table to speed up linkage




Dynamically Linked:

compile with position-independent encoding:

gcc -fPIC -c mylib.c -o mylib.o 

-c means compile only, no linking
-fPIC means generate Positional Independant Encoding:
normally, compiled code can contain absolute addresses :jump to 0x102911281
PIC means use relative addressing (from RIP or something) [rip + offset_to_global]
Or Global Offset Table (GOT) (read process notes for more info) [rip + offset_to_GOT_entry]
basically, a table of addresses for global variables and external symbols (entities the linker must resolve, like functions in .so/.a, global variables, and external references)
dynamic linker resolves symbols and writes their actual addresses in at runtime into the table

gcc -shared -o libmylib.so mylib.o 




Step 3: Use said Library

main.c

#include "mylib.h"

int main() {
    hello();
    return 0;
}

Compile + Link:

Static Linking Example:
gcc main.c -L. -lmylib -o app

-L. means look in current directory for libraries
-lmylib -> find libmylib.a and link it

Dynamic linking

gcc main.c -L. -lmylib -o app
(same command, but prefers .so if possible)

Step 5:
KEY FOR RUNTIME:
when running
./app, might get: 
"error while loading shared libraries: libmylib.so: cannot open shared object file"
reason: OS doesnt know where to look for the library (its not in /lib or /usr/lib)
(NOTE: header file in /usr/include on Linux or /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/stdio.h)

Fixes:

Temporary Fix: make the path clear:
LD_LIBRARY_PATH=. 
./app

Make path clear to linker

gcc main.c -L. -lmylib -Wl,-rpath=. -o app

-Wl, means send the following to the linker
-rpath=.  means at runtime, look at current directory for paths


More permanent fix:
sudo cp libmylib.so /usr/lib //copies it to the directory full of libraries
sudo ldconfig //runs the tool that updates the shared library cache. scans /lib /usr/lib and registers those libs
so the linker can easily find them

this changes your linking command to make it much smaller

gcc main.o -lmylib -o app (no -L. because /usr/lib is standard, and no -Wl,-rpath=. because no need)

NOTE: might be safer to put in /usr/local/lib, as /usr/lib meant for package managers, not good to put user
files there



Lastly about the header:
-I. used to tell it where to find the headers that it needs for the main, ie whenever a #include pops up
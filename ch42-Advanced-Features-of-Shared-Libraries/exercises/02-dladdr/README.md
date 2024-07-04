# Exercise 42.2

Add a `dladdr()` call to the program in Listing 42-1 (`dynload.c`) in order to retrieve information
about the address returned by `dlsym()`. Print out the values of the fields of the returned `Dl_info`
structure and verify that they are as expected.

## Solution

To expose the declaration of `dladdr()`, I used the `_GNU_SOURCE` feature flag:

```c
#define _GNU_SOURCE
#include <dlfcn.h>
```

Then I declared a variable of type `Dl_info`:

```c
	Dl_info info;
```

Finally, using the function pointer `funcp` returned by `dlsym()`, I got the information of interest:

```
	if (dladdr(funcp, &info) == 0)
		fatal("dladdr: %s", dlerror());
	
	printf("Library Path: %s\nLibrary Base Address: %p\nSymbol Name: %s\nSymbol Value: %p\n",
				info.dli_fname, info.dli_fbase, info.dli_sname, info.dli_saddr);
```

We can then run `make` to build the program. Note that the `Makefile` was modified to link
against *libdl*  using the `-ldl` linker option:

```bash
make
```

To verify, I copied `mod1.c` from `tlpi_dist/shlibs/demo/mod1.c` to build the shared library:

```bash
# Create a directory for the shared library
mkdir demo
mv mod1.c demo/
cd demo/

# Compile the sole object file for the shared library
gcc -g -c -Wall -fPIC mod1.c

# Create shared library 
gcc -g -shared -o libmod.so.1.0.0 -Wl,-soname,libmod.so.1 mod1.o
# Create symbolic link for soname
ln -s libmod.so.1.0.0 libmod.so.1
# Create symbolic link for linker name
ln -s libmod.so.1 libmod.so
cd ..
```

Then we can run `dynload` as follows:

```bash
./dynload demo/libmod.so x1
```

The output is:

```
Library Path: demo/libmod.so
Library Base Address: 0x769a2e27b000
Symbol Name: x1
Symbol Value: 0x769a2e27c119
Called mod1-x1
```

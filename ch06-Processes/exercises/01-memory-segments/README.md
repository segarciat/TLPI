# Exercise 06-01

Compile the program in Listing 6-1 (`mem_segments.c`), and list its size
using `ls -l`. Although the program contains an array (`mbuf`) that is
around 10 MB in size, the executable file is much smaller than this.
Why is this?

## Solution

When compiled with `gcc` (no flags otherwise specified) and listed
with `ls -l`:

```bash
gcc mem_segments.c
ls -l
```

we get the output

```
-rwxr-xr-x 1 segarciat segarciat 16256 Mar 14 14:55 a.out
-rw-r--r-- 1 segarciat segarciat  2010 Mar 14 14:54 mem_segments.c
```

From the discussion on Section 6.3 on the memory layout of a process,
the memory allocated to a process consists of parts called segments.
Among them is the *uninitialized data segment* (or *zero-initialized
data segment*). It conatins global and static variables that are explicitly
not initialized. In the case of `mbuf` in the main function, we have:

```c
/* more code */
static char mbuf[10240000];
/* more code */
```

Since it is declared static and not otherwise initialized, it belongs
in the uninitialized data segment. As discussed in the text, when a
program is stored on disk, it is not necessary to allocate space for
initialized data. Instead, the program records the location and size
required for the uninitialized data, and allocates it at run time
with 0s when the program is loaded into memory.

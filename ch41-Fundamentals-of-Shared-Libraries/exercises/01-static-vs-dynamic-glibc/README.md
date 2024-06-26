# Exercise 41.1

Try compiling a program with and without the `-static` option, to see the difference in size
between an executable dynamically linked with the C library and one that is linked against the static
version of the C library.

## Solution

As discussed in Section 41.13, by default, where the linker has a choice of a shared and a static
library of the same name, the shared version of the library is used. To force usage of the static
versions of the library, we may do one of the following:

1. Specify the pathname of the static library (including the `.a` extension) on the `gcc` command line.
2. Specify the `-static` option to `gcc`.
3. Use the `gcc` options `-Wl,-Bstatic` and `Wl,-Bdynamic` to explicitly toggle the linker's choice
between static and shared libraries. These options can intermingled with `-l` options on the `gcc`
command line. The linker processes the options in hte order in which they are specified.

I used option 2, employing the `-static` option. I created `hello_dynamic` and `hello_static`:

```bash
gcc -o hello_dynamic hello.c
ldd hello_dynamic
```

The output of `ldd` shows dynamic libraries linked against `hello_dynamic`, including the dynamic
linker itself, and `libc`:

```
	linux-vdso.so.1 (0x00007ffc754ee000)
	libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007406d9a00000)
	/lib64/ld-linux-x86-64.so.2 (0x00007406d9cdc000)
```

I tried this again, this time with `-static`:

```bash
gcc -static -o hello_static hello.c
ldd hello_static
#	not a dynamic executable
ls -l
```

The `ldd` command outputs *not a dynamic executable* in this case. Using `ls -l` shows
that `hello_dynamic` is about 60 times larger:

```
total 90
-rw-rw-r-- 1 sgarciat sgarciat     84 Jun 26 15:21 hello.c
-rwxrwxr-x 1 sgarciat sgarciat  15960 Jun 26 15:26 hello_dynamic
-rwxrwxr-x 1 sgarciat sgarciat 900344 Jun 26 15:26 hello_static
-rw-rw-r-- 1 sgarciat sgarciat    238 Jun 26 15:21 README.md
```

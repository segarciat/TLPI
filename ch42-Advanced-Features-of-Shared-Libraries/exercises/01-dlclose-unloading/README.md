# Exercise 42.1

Write a progarm to verify that if a library is closed with `dlclose()`, it is not unloaded if any of
its symbols are used by another library.

## Solution

I created two sample libraries. The first is *libmthbasic*, meant to represented a small library
with elementary math operations as well as constants. It consists of the single file `math_constants.c`
which defines a constant `PI` as well as a function `int add(int, int)`.

```bash
# Compile math_constants into an object file
gcc -g -c -fPIC -Wall math_constants.c
```

I've used the following `gcc` options:

- `Wall`: Generate warnings.
- `-g`: Include debugging information in the compiled program.
- `-c`: Compile and assemble, but do not link
- `fPIC`: Generate position-independent code. This is necessary for creating a shared library on most UNIX
implementations. Changing the way the compiler generates code for operations such as accessing global, static,
and external variables, accessing string constants, and taking the addresses of functions.

This creates `math_constants.o`. We then use this to create a shared library:

```bash
mkdir libmthbasic
cd libmthbasic
# Create v1 of library with the given soname from the object file
gcc -g -shared -Wl,-soname,libmthbasic.so.1 -o libmthbasic.so.1.0.0 math_constants.o
# Create link from soname to the actual library
ln -s libmthbasic.so.1.0.0 libmthbasic.so.1
# Create linker name that points to soname
ln -s libmthbasic.so.1 libmthbasic.so
# Go back to root of exercise
cd ..
```

An explanation of the commands is in order (see Table 41-1 on page 846):
- `-shared`: Instruct `gcc` that we are constructing a shared library.
- `-Wl,-soname,libmthbasic.so.1`: Provides a soname for this library. There is one per major version of
a library, and it is embedded in an executable at link time. It is used at runtime by the dynamic linker
via a symbolic link with the same name that points to the most up-to-date real name of the library.
- `-o libmthbasic.so.1.0.0`: The actual file holding the library code. There is one instance per
major-plus-minor version of the library.
- `ln -s libmthbasic.so.1.0.0 libmthbasic.so.1`: Creates a symbolic link matching the soname specified by
the linking command.
- `ln -s libmthbasic.so.1 libmthbasic.so`: Creates a *linker name*. This is a symbolic link to the real name
or (more usually) latest soname. There is a single instance of this, and it allows construction of
version-independent linker commands.

Next we can compile *libmthops*:

```bash
mkdir libmthops
cd libmthops
gcc -g -Wall -c -fPIC math_ops.c
gcc -g -shared -Wl,-soname,libmthops.so.1 -o libmthops.so.1.0.0 -Wl,-rpath,../libmthbasic -L../libmthbasic math_ops.o -lmthbasic
ln -s libmthops.so.1.0.0 libmthops.so.1
ln -s libmthops.so.1 libmthops.so
```

The command is similar to before, but there are a few new options:

- `-Wl,-rpath,../libmthbasic`: The `-rpath` linker option copies the string `../libmthbasic` into the
run-time library path (*rpath*), so that at runtime, the dynamic linker will also search this directory
when resolving shared library references.
- `-L../libmthbasic`: The `-L` option informs the linker of directory where to search for additionally libraries
besides the defaults (e.g., `/lib`, `/usr/lib`). This happens to be the same as the path given to `-rpath`, but
in general it need not me.
- `-lmthbasic`: The `-l` option allows us to specify a library name, without the `lib` prefix or the `.so` suffix.

Now we can compile

```bash
gcc -o main main.c -ldl
```

Note that we are not linking `main.c` against either library. The program uses the *dlopen* API from
*libdl* to dynamically load both libraries (hence the `-ldl` command-line option). If we run `./main`,
it fails:

```
dlopen: libmthbasic.so: cannot open shared object file: No such file or directory
```

This is because the dynamic linker looks in a set of predefined directories such as `/lib` and `/usr/lib`
by default, but not in our local `libmthbasic` nor `libmthops` directories. We can use the environment
variable `LD_LIBRARY_PATH` to specify a colon-separated list of directories where the dynamic linker
will look before any standard directory.

```bash
LD_LIBRARY_PATH=./libmthbasic:./libmthops ./main
```

The output is:

```
libmthbasic loaded
Loaded libmthops
Closing libmathbasic first
Unloading libmathops
libmthbasic unloading
```

As you can see, although *libmthbasic* is closed first, it is *libmthops* that is unloaded first.
This is because *libmthops* uses symbols in *libmthbasic*. Therefore, *libmthbasic* is not
actually unloaded; the `dlclose()` call to it in `main` merely serves to decrease the reference
the count by 1. When `dlclose()` is then called on `libmthops`, its destructor is called, and
at that point `libmthbasic` can be unloaded as well.

We can compile with a command-line argument (anything is fine) and this reverses the order
of the `dlclose()` call:

```bash
LD_LIBRARY_PATH=./libmthbasic:./libmthops ./main any_argument
```

The output is

```
libmthbasic loaded
Loaded libmthops
Closing libmathops first
Unloading libmathops
libmthbasic unloading
```

As you can see, *libmathops* is close first again. This is because no other libraries hold references
to symbols defined in it, including *libmthbasic*, so it can be safely unloaded and its destructor
runs. Then when `dlclose()` is subsequently called on *libmthbasic*, it unloads, as expected.

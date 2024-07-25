# Exercise 49-1

Write a program, analogous to *cp(1)*, that uses *mmap()* and *memcpy()* calls (instead
of *read()* or *write()*) to copy a source file to a destination file. (Use *fstat()* to
obtain the size of the input file, which can then be used to size the required memory
mappings, and use *ftruncate()* to set the size of the output file.)

## Solution

The program can be compiled and run as follows:

```bash
# Compile
gcc -Wall -o cp cp.c
./cp -h
./cp README.md COPY_README.md
diff README.md COPY_README.md
echo $?
```

The output of `echo $?` should be 0 since `diff` outputs 0 when its put input files are equal.
The following are some cautions I observed or wrestled with.

1. It's important to remember that the mode on the output file should be `O_RDWR`, not `O_WRONLY`.
As discussed in Section 49.4.4, no combination of memory protections, not even just `PROT_WRITE`,
is compatible with a file opened with `O_WRONLY`. Thus, the protection specified should be
`PROT_READ | PROT_WRITE`.

2. The memory mapping for the output file should be a *shared memory mapping* (`MAP_SHARED`) and
*not* a *private memory mapping* (`MAP_PRIVATE`). As discussed in Section 49.2, changes to the
region corresponding to a private memory-mapped file are not carried through to the underlying file.
I had forgotten about this and was initially using `MAP_PRIVATE`, so the output file constants were
not what I expected.

3. In addition to getting the file size by using `fstat()`, I also leveraged the stat buffer to
copy the file permissions to the output file. I was careful to call `umask(0)` to get the right
permissions.

4. I truncate the output file with `ftruncate()` to the right size prior to creating the memory
mapping for the output file because otherwise the kernel will deliver a `SIGBUS` to the process
when *memcpy()* attempts to copy bytes into the region. This issue is discussed in Section 49.4.3.

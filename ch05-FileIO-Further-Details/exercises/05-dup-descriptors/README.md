# Exercise 05-05

Write a program to verify that duplicated file descriptors share a file offset and open file
status flags.

## Solution

The current implementation accepts a single command-line argument which is meant to be a file.
It is opened for reading with `open`, and the returned descriptor is used to create a duplicate
descriptor is created with `dup()`.

Recall that the file offset is set to 0 at the start. My implementation sets the offset to
end-of-file using `lseek(fd, 0, SEEK_END)` on the initial fd, then compared against the result of
the current offset on the duplicate fd, this time with a call `lseek(dupfd, 0, SEEK_CUR)`.

For the open file status flags, I was careful to use the flags returned by `fcntl(fd, F_GETFL)`,
and not the ones passed initially to `open()`, when comparing against the status flag
returned by `fcntl(dupfd, F_GETFL)`.

```c
make
./verify_dup_descriptors README.md
```

Sample output:

```
Flags and offsets match!
Flags: 32768
Offsets:868
```

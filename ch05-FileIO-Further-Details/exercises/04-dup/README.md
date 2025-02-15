# Exercise 05-04

Implement `dup()` and `dup2()` using `fcntl()` and, where necessary, `close()`.
(You may ignore the fact that `dup2()` and `fcntl()` return different `errno`
values for some error cases.) For `dup2()`, remember to handle the special case
where `oldfd` equals `newfd`. In this case, you should check whether `oldfd`
is valid, which can be done, for example, by checking if `fcntl(oldfd, F_GETFL)`
succeeds. If `oldfd` is not valid, then the function should return `-1` with
`errno` set to `EBADF`.

## Solution

I implemented both operations, but of course I could not guarantee the atomicity
of `dup2`. I created a program to test `my_dup2`. It creates a file whose
name is the first argument in the command line. It duplicates the file descriptor
of this new file on stdout. The result is that `STDOUT_FILENO`, the file descriptor
associated with stdout, now refers to the same open file description in
the (system-wide) open file table, rather than the terminal. Therefore, a write
to stdout should be performed on the file instead.

To test it, run

```bash
make
./main tfile
cat tfile
```

The contents are correctly:

```
Hello, world
```

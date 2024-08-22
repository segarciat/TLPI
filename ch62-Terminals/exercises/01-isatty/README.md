# Exercise 62-1

Implement *isatty()*. (You may find it useful to read the description of *tcgetattr()*
in Section 62.2.)

## Solution

The *isatty()* function enables us to determine whether a file descriptor *fd*
is associated with a terminal (as opposed to  another file type). The *tcgetattr()*
function fails with error `ENOTTY` if *fd* is a file descriptor that does not
refer to a terminal. This makes the implementation clear.

I additionally used `fcntl(fd, F_GETFD)` as a means to determine whether *fd*
was a terminal. The reason is that the manual for *tcgetattr()* does not clearly
say if or whether or when error numbers `ENOTTY` or `EBADF` are set on failure.

The accompanying main program simply checks whether standard input refers to
a terminal:

```bash
gcc -Wall -o main main.c
./main
```

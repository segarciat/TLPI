# Exercise 24.2

Write a program to demonstrate that after `vfork()`, the child process can close a file
descriptor (e.g., descriptor 0) without affecting the corresponding file descriptor in
the parent.

## Solution

The program `vfork_fds.c` that I work uses `vfork()`, and closes file descriptor 0,
corresponding to standard input, with the `close()` system call. Then when the parent
process resumes, it tries to use file descriptor 0 by reading input with `read()`, which
it the echos back with `write()`.

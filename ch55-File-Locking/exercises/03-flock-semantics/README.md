# Exercise 55-3

Write a program to verify the statements made in Section 55.2.1 regarding the semantics
of inheritance and release of *flock()* locks.

## Solution

The semantics of *flock()* described in Section 55.2.1 are as follows:

- A file lock is associated with an open file description, rather than the file descriptor or
the file (i-node) itself. Thus, if a file descriptor is used to lock a file, and it is then
duplicated (via *dup()*, *dup2()*, or an *fcntl()* `F_DUPFD` operation), then the new file
descriptor points to the same file description, and hence it refers to the same lock. Similarly,
if a *fork()* spawns a child process, then the child obtains a duplicate of its parent's file
descriptors, and these point to the same file descriptions. As a result, the new descriptor
(e.g., the child's file descriptor) can be used to unlock the file. Since the first file
descriptor points to the same open file description, the change affects it, too.

- If a process opens the same file twice, and it obtains an exclusive lock on one of its two
file descriptors, then an attempt to get an exclusive lock on the other file descriptor will
block. Thus, a process can lock itself out of a file.


My program verifies both of these semantics. Compile with:

```bash
gcc -Wall flock_semantics.c
./a.out README.md
```

The output was:

```
Obtained first exclusive lock on README.md
Attempting to obtain second exclusive lock on README.md
flock() timed out as expected while blocked waiting on exclusive lock
[49718] Forking child while still holding lock
[49725] Child releasing inherited lock
[49718] Reaped child, retrying to acquire lock with the second file description
[49718] Successfully acquired the exclusive lock
```

# Exercise 05-03

This exercise is designated to demonstrate why the atomicity guaranteed by opening
a file with the `O_APPEND` flag is necessary. Write a program that takes up to
three command-line arguments:

```bash
atomic_append filename num-bytes [x]
```

This program should open the specified filename (creating it if necessary) and
append `num_bytes` bytes to the file by using `write()` to write a byte at a time.
By default, the program should open the file with the `O_APPEND` flag, but if a third
comand-line argument (`x`) is supplied, then the `O_APPEND` should be omitted,
and instead the program should perform a `lseek(fd, 0, SEEK_END)` call before
each `write()`. Run two instances of this program at the same time without the
`x` argument to write 1 million bytes to the same file.

```bash
atomic_append f1 1000000 & atomic_append f2 1000000
```

Repeat the same steps, writing to a different file, but this time specifying the
`x` argument

```bash
atomic_append f2 1000000 x & atomic_append f2 1000000 x
```

List the sizes of the files `f1` and `f2` using `ls -l` and explain the difference.

## Solution

After running the commands as indicated

```bash
./atomic_append f1 1000000 & ./atomic_append f1 1000000
./atomic_append f2 1000000 x & ./atomic_append f2 1000000 x
```

They runs successfully and running `ls -l` shows that they are different sizes:

```
-rw------- 1 segarciat segarciat 2000000 Mar 13 18:01 f1
-rw------- 1 segarciat segarciat 1003941 Mar 13 18:01 f2
```

The differences can be attributed to the fact that there is a race condition when
the `x` argument is provided. Consider the first case:

```bash
./atomic_append f1 1000000 & ./atomic_append f1 1000000
```

Since the processes are different, the file descriptors they have for `f1`
points to different file descriptions in the system-wide open file table. As a result,
their file offsets are independent. However, the inode of both descriptions corresponds
to the same inode entry, for `f1`. As a result, the writes are made to the same file.

Since the `x` argument is not specified, both file descriptors have `O_APPEND` as part
of their status flags. Therefore, rather than the normal behavior where a call to `write()`
writes to the location specified by the file offset in the open file description entry,
each `write()` call seeks to the end of the file and then writes the requested bytes in an
atomic fashion. As a result, both process always write to the end of the file. The order
in which the content appear is not guaranteed, but the file is guaranteed to have 2000000
bytes.

Consider now the second case:

```bash
./atomic_append f2 1000000 x & ./atomic_append f2 1000000 x
```

Most details are the same as in the first case: the file descriptors in the two processes
point to different open file descriptions in the system-wide open-file table, so
their file offsets are independent, but they have the same inode entry because they both
refer to file `f2`.

However, since `x` is specified, neither descriptor has the `O_APPEND` status flag, so
each `write()` is done at the location specified by the status offset field in the
respective open file description entries referred to by their file descriptors.
Moreover, the `x` argument also causes each `write()` is preceded by a call to
`lseek(fd, 0, SEEK_END)`, but these two calls do not occur atomically. That is,
it's possible for the following to happen:

1. Process 1 calls `lseek(fd, 0, SEEK_END)`, updating the file offset in its
   independent entry on the open file table.

2. The kernel scheduler decides that the time slice for the process has expired.
   It suspends process 1 and gives control to process 2.

3. Process 2 calls `lseek(fd, 0, SEEK_END)`, updating its file offset to the same
   location as process 1.

4. Process 2 calls `write()`, writing to the current file offset indicated in its own
   open file description entry, and updating its own offset to the next position.

5. The scheduler preempts process 2 and transfers control back to process 1.

6. Process 1 calls `write()`, writing to the current file offset indicated in its own
   open file description; this is the old position that process 2 wrote to. In this
   case, process 1 overwrites the content written by process 2.

Since the result depends on the order in which the operations happen between process 1
and process 2, we have a race condition. Hence the file `f2` may be shorter compared to
`f1` because of potential overwrites.

# Exercise 13-05

The command `tail [-n num] file` prints the last *num* lines (ten by default)
of the named file. Implement this command using the I/O system calls (`lseek()`,
`read()`, `write()`, and so on). Keep in mind the buffering issues described in
this chapter, in order to make the implementation efficient.

## Solution

In terms of efficiency, I used a reasonably large buffer size to minimize the
number of necessary `read` and `write` system calls.

My approach was to read the file backwards, starting at the end, looking for newline
characters (`\n`). To do so, I used `lseek(fd, 0, SEEK_END)` to find the offset to
reach the end of the file. Then I read read offset windows as follows:

```
1. Read bytes in the offset interval [end - BUFSIZ, end]
2. Read bytes in the offset interval [end - 2 * BUFSIZ, end - BUFSIZ]
3. Read bytes in the offset interval [end - 3 * BUFSIZ, end - 2 * BUFSIZ]
...
```

Each time I read up to `BUFSIZ` bytes, I looked for the newline characters, and stopped
the search when I had encountered sufficiently many newlines.

### Compile and Run

```bash
make

# Use the source file itself to test
./tail -n 15 tail.c
```

You can also compare it with the `tail` command:

```bash
./tail tail.c > mytail
tail tail.c > theirtail
diff mytail theirtail
```

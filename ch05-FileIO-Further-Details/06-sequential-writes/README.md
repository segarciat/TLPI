# Exercise 05-06

After each of the calls to `write()` in the following code, explain what the content
of the output file would be, and why:

```c
fd1 = open(file, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
fd2 = dup(fd1);
fd3 = open(file, O_RDWR);
write(fd1, "Hello,", 6);
write(fd2, " world", 6);
lseek(fd2, 0, SEEK_SET);
write(fd1, "HELLO,", 6);
write(fd3, "Gidday", 6);
```

## Solution

Since `fd2` was created with `dup(fd1)`, it follows that `fd1` and `fd2` refer to
the same open file description in the system-wide open file table. However, since
`fd3` was created with `open`, its open file description is different. All
three file descriptors have the same underlying inode entry.

Assuming that all system calls succeed:

1. `write(fd1, "Hello", 6)`: After this call, the file will have the text `Hello,`.

2. `write(fd2, " world", 6)`: Since `fd2` shares the same file offset as `fd1`, it will
    pick up where `fd1` left off, and now the file contents will be `Hello, world`.

3. `lseek(fd2, 0, SEEK_SET)`: The file offset is set to the beginning of the file,
    as viewed by `fd1` and `fd2`.
4. `write(fd1, "HELLO,", 6)`: Since the file offset is now at 0 for `fd1`, it replaces
	`Hello` with `HELLO`, so the file contents are now `HELLO, world`.
5. `write(fd3, "Gidday", 6)`: Since `fd3` has not been used to write, and since its open
    file description is separate from that of `fd1` and `fd2`, it follows that its file
	offset is still 0. Hence, it overwrites `HELLO` and the contents of the file are now
	`Gidday, world`.

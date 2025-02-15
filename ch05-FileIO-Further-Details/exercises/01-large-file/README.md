# Exercise 05-01

If you have access to a 32-bit Linux system, modify the program in Listing 5-3 to use
the standard file I/O system calls (`open()` and `lseek()`) and the `off_t` data type.
Compile the program with the `_FILE_OFFSET_BITS` macro set to 64, and test it to show
that a large file can be successfully created.

## Solution

I tested `large_file` on a Raspberry PI, and I used the `uname -m` command to learn
what its kernel architecture was. Its output was `armv7l`, which is 32-bit.
The file was created successfully in spite of being at least 10GB in size.

```bash
make

# 10000000 bytes, or about 10 GB
./large_file tfile 10000000

# See information about file
ls -l tfile

make clean
rm tfile
```

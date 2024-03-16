# Exercise 05-01

If you have access to a 32-bit Linux system, modify the program in Listing 5-3 to use
the standard file I/O system calls (`open()` and `lseek()`) and the `off_t` data type.
Compile the program with the `_FILE_OFFSET_BITS` macro set to 64, and test it to show
that a large file can be successfully created.

## Solution

To build the project, note that the `makefile` assumes the existence of `tlpi_dist`
at the root of this git repository, provided as a relative path. Note that the
directory `tlpi_dist` is in the `.gitignore`, so it is not included, but you can
obtain it from the book's site.

I tested `large_file` on a Raspberry PI, and I used the `uname -m` command to learn
what its kernel architecture was. Its output was `armv7l`, which is 32-bit.
The file was created successfully in spite of being at least 10GB in size.

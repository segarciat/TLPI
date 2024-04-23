# Exercise 18.7

Write a program that uses `nftw()` to traverse a directory and finishes by printing out counts
and percentages of the various types (regular, directory, symbolic link, and so on) of the files.

## Solution

My approach was to use `nftw()` with the `FTW_PHYS` flag to avoid dereferencing symbolic links.
Unfortunately, I could not find a way around using global variables for the counts. My implementation
accepts a single argument with a path name, or none, in which case the current directory is processed.

```bash
gcc -o cft count_file_types.c
./cft
```

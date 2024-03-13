# Exercise 04-02

Write a program like `cp` that, when used to copy a reuglar file that contains
holes (sequences of null bytes), also creates corresponding holes in the target
file.

## Solution

The implementation here determines whether a hole in the file was found by comparing
the current position and end position of the input file by calling `lseek`. If the
values are equal, then we've reached the end-of-file and we stop reading input. Otherwise,
it's a file hole, so we introduce a hole in the output file, reset the input file position,
and continue reading input.

```bash
# Build executable using makefile
make

# sample input files
cat "hi" > hello.txt
cat "bye" > goodbye.txt

# Creates greetings.txt with the same contents as in hello.txt
./copy hello.txt greetings.txt

# Overwrite contents in goodbye.txt with contents of hello.txt
./copy hello.txt goodbye.txt
```

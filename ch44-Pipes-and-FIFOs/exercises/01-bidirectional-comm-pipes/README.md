# Exercise 44.1

Write a program that uses two pipes to enable bidirectional communication between a parent and child
process. The parent process should loop reading a block of text from standard input and use one of
the pipes to send the text to the child, which converts it to uppercase and sends it back to the
the parent via the other pipe. The parent reads the data coming back from the child and echoes it
on stnadard output before continuing around the loop once more.

## Solution

Compile with:

```bash
gcc bidir_pipes_upper.c
```

Then run on any text file:

```bash
./a.out < bidir_pipes_upper.c
./a.out < README.md
```

Or run standalone and provide input:

```bash
./a.out
```

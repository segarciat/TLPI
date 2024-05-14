# Exercise 27.3

What output would we see if we make the following script executable and `exec()` it?

```bash
#!/bin/cat -n
Hello world
```

## Solution

In Section 27.3, we learned that when `execve()` detects that the file it has been given
commences with the 2-byte sequence `!#`, then then it extracts the remainder of the line
(pathname and argument), and execs the interpreter as

```
interpreter-path [optional-arg] script-path arg...
```

Above, the interpreter path is `/bin/cat` and the optional-arg is `-n`, which tells it
to print line numbers. The `cat` program displays its input to standard output, so the
program will read the entire file and print both lines, each preceded by the line numbers.
The expected output is:

```
1 #!/bin/cat -n
2 Hello world
```

To verify this I also write `test.c` and `test.cat`.

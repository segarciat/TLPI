# Exercise 18.5

Implement a function that performs the equivalent of `getcwd()`. A useful tip for solving
this problem is that you can find the name of the current working directory by using
`opendir()` and `readdir()` to walk through each of the entries in the parent directory (`..`)
to find an entry with the same i-node and device number s the current working directory
(i.e., respectively, tthe `st_ino` and `st_dev` fields in the `stat` structure returned by
`stat()` and `lstat()`). Thus, it is possible to construct the directory path by walking up
the directory tree (`chdir("..")`) one step at a time and performing such scans. The walk can
be finsihed when the parent directory is same as the current working directory (recall that `/..`
is the same as `/`). The caller should be left in the same directory in which it started, regardless
of whether your `getcwd()` function succeeds or fails (`open()` plus `fchdir()` are handy for
this purpose).

## Exercise

I followed the tips to solve this question. I tried paying closed attention to ensuring that resources
such as directory handles were properly closed. I built the path string starting from the end of
the supplied buffer. At the end, I made sure to re-copy the path starting at the beginning. I did not
handle a few errors, such as checking `ENOENT`, though I did make sure to check the status of most call
to ensure it succeeded, and aborted if not.

```bash
gcc -o cwd main.c getcwd.c
./a.out
```

I ran it from several paths and ascertained that the output directory string was my current working directory.

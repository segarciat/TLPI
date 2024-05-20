# Exercise 28.1

Write a program to see how fast the `fork()` and `vfork()` system calls are on your system.
Each child process should exit immediately, and the parent should `wait()` on each child
before creating the next. Compare the relative differences for these two system calls with
those of table 28.3. The shell built-in command `time` can be used to measure the execution
time of a program.

## Solution

I wrote a program that takes in a single argument for the number of runs to try. By default
it tests `fork()`, but if compiled with `-DVFORK`, it will test `vfork()` instead.

```bash
# Test fork()
gcc fork_vfork_speed.c
time ./a.out 100000
```

The results were:

```
real	0m7.676s
user	0m3.950s
sys	0m3.959s
```

```bash
# test vfork()
gcc fork_vfork_speed.c -DVFORK
time ./a.out 100000
```

This time I observed the following:

```
real	0m1.588s
user	0m0.550s
sys	0m1.134s
```

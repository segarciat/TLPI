# Exercise 36.1

Write a program that shows that the *getrusage()* `RUSAGE_CHILDREN` flag retrieves information
about only the children for which a *wait()* call has been performed. (Have the program create
a child process that consumes CPU time, and then have the parent call *getrusage()* before and
after calling *wait()*).

## Solution

```bash
# Compile and link against realtime library to use POSIX clocks
gcc rusage_children.c -lrt

# Run
./a.out
```

The output is as follows, verifying the claim in the problem:

```
Child [63663]: Will consume CPU time
Child [63663]: Done consuming CPU time, signaling parent
Parent [63662]: Child used 0 seconds and 0 nanoseconds. Reaping...
Parent [63662]: Child used 0 seconds and 1000393 nanoseconds.
```

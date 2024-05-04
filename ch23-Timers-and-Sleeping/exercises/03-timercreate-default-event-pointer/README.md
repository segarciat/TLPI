# Exercise 23.3

Write a program to show that if the `evp` argument to `timer_create()` is specified as
`NULL`, then this is equivalent to specifying `evp` as a pointer to a `sigevent` structure
with `sigev_notify` set to `SIGEV_SIGNAL`, `sigev_signo` set to `SIGALRM`, and
`si_value.sival_int` set to the timer ID.

## Solution

To use the use the POSIX timer API on Linux, must compile with `-lrt` to link against
the *librt* (realtime) library.

```bash
gcc -o try_null_evp timercreate_evp_null.c -lrt
./try_null_evp
```

Then run, and this is the expected output:

```bash
Timer ID: 0
Setting timer for 3 seconds...
si_signo=14 (SIGALRM), si_code=-2 (SI_TIMER), si_value=0
```

# Exercise 23.4

Modify the program in Listing 23.5 (`ptmr_sigev_signal.c`) to use `sigwaitinfo()` instead
of a signal handler.

## Solution

I eliminated the handler and added the alarm of interest to the process signal mask early in the execution
of the process. The I replaced the body of the last `for` loop, which initially only had a call to `pause()`,
with a call to `sigwaitinfo()`. I moved the `printf()` statements in the old signal handler, and I accessed
the same data through the use of the `siginfo_t` value gotten through the second aragument of `sigwaitinfo()`.

```bash
make
./ptmr_sigev_signal 2
```

Sampel output:

```
Timer ID: 0 (2)
[02:08:04] Got signal 64
    *sival_ptr         = 0
    timer_getoverrun() = 0
^C # Interrupt and end
```

# Exercise 22.3

Section 22.10 stated that accepting signals using `sigwaitinfo()` is faster than the use
of a signal handler plus `sisuspend()`. The program `signals/sig_speed_sigsuspend.c`,
supplied in the source code distribution for this book, uses `sigsuspend()` to alternately
send signals back and forth between a parent and a child process. Time the operation of
this program to exchange one million signals between the two processes. (The number of signals
to exchange is provided as a command-line argument to the program.) Create a modified version
of the program that instead uses `sigwaitinfo()`, and time that version. What is the speed
difference between the two programs?

## Solution

The existing call to `sigsuspend()` was prepared as follows:

```c
sigset_t blockMask, emptyMask;

/* Block the signal before forking */
sigemptyset(&blockMask);
sigaddset(&blockMask, SIGUSR1); 
sigpromask(SIG_SETMASK, &blocMask, NULL);

/* Unblock signal and suspend until it is received */
sigemptyset(&emptyset);
sigsuspend(&emptyset);
```

To use `sigwaitinfo()`, we still add `SIGUSR1` to the process signal mask, but
instead of specifying a new process mask before suspending, we instead specify
a set of signals to wait for. Notice that the signals do not need to be unblocked;
they will become pending, at which point `sigwaitinfo()` will return. I omitted
the second argument by passing `NULL`, since we are not doing anything with it
in the handler:

```c
sigset_t blockMask;

/* Block the signal before forking */
sigemptyset(&blockMask);
sigaddset(&blockMask, SIGUSR1); 
sigpromask(SIG_SETMASK, &blocMask, NULL);

/* Suspend until any signals in the blockMask set is pending */
sigwaitinfo(&blockMask, NULL);
```

### Compiling and running:

Using the provided `Makefile`, run

```bash
make sig_speed_sigsuspend
make sig_speed_sigwaitinfo
```

Then I made it so that the parent and child send each other 1 million, and then 10 million
signals:

```
# 1 million signals
$ time ./sig_speed_sigsuspend 1000000

real	0m5.832s
user	0m0.313s
sys		0m2.430s
$ time ./sig_speed_sigwaitinfo 1000000

real	0m4.609s
user	0m0.163s
sys		0m1.995s


# 10 million signals
$ time ./sig_speed_sigsuspend 10000000

real	1m6.837s
user	0m2.913s
sys	0m27.365s

$ time ./sig_speed_sigwaitinfo 10000000

real	0m59.323s
user	0m2.097s
sys	0m24.298s
```

Accounting only for the user process and system process time, there's about a 27% improvement
in the case for 1 million signals, and about 14% improvement for the case using 10 million
signals.


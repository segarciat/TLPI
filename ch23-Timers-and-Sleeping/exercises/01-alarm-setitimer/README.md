# Exercise 23.1

Although `alarm()` is implemented as a system call within the Linux kernel, this is
redundant. Implement `alarm()` using `setitimer()`.

## Solution

My implementation is very simple; it simply calls `setitimer()` to establish a timer of type
`ITIMER_REAL`, causing a `SIGALRM` to be generated after the specifier number of seconds.
It returns the previous timer's remaining expiration seconds.

I also added a main routine in `main.c` which sets an alarm for 5 seconds, displaying
seconds passed until the `SIGALRM` is caught and handled.

```bash
gcc -o s_alarm main.c alarm.c
./s_alarm
```

The following is sample output:

```
Setting alarm for 5 seconds...
1
2
3
4
5
Got alarm!
```

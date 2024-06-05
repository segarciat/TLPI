# Exercise 35.3

Write a program that places itself under the `SCHED_FIFO` scheduling policy and then creates
a child process. Both processes should execute a function that causes the process to consume
a maximum of 3 seconds of CPU time. (This can be done by using a loop in which the *times()*
system call is repeatedly called to determine the amount of CPU time so far consumed.) After
each quarter of a second of consumed CPU time, the function could print a message that displays
the process ID and the amount of CPU so far consumed. After each second of consumed CPU time,
the function should call *sched_yield()* to yield the CPU to the other process. (Alternatively,
processes could raise each other's scheduling priority using *sched_setparam()*.) The program's
output should demonstrate that the two processes alternately execute for 1 second of CPU time.
(Note carefully the advice given in Section 35.3.2 about preventing a runaway realtime process
from hogging the CPU.)

> If your system has multiple CPUs, then, in order to demonstrate the behavior described in this
exercise, you will need to confine all processes to a single CPU. This can be done by calling
*sched_setaffinity()* before creating the child process or from the command line using the
*taskset* command.

## Solution

After compiling, we must run using `sudo` because by default a process may use the `SCHED_OTHER` policy,
and it needs to be privileged to switch to the `SCHED_FIFO` policy.

```bash
# Link against realtime library (POSIX clocks)
gcc sched_fifo_alternate.c -lrt

# Use superuser privilege to change from default round-robin to realtime scheduling policy
sudo ./a.out
```

The output is the following:

```
Process [60384]: Starting
Process [60384]: 0.25, yielding CPU
Process [60385]: Starting
Process [60385]: 0.25, yielding CPU
Process [60384]: 0.50, yielding CPU
Process [60385]: 0.50, yielding CPU
Process [60384]: 0.75, yielding CPU
Process [60385]: 0.75, yielding CPU
Process [60384]: 1.00, yielding CPU
Process [60385]: 1.00, yielding CPU
Process [60384]: 1.25, yielding CPU
Process [60385]: 1.25, yielding CPU
Process [60384]: 1.50, yielding CPU
Process [60385]: 1.50, yielding CPU
Process [60384]: 1.75, yielding CPU
Process [60385]: 1.75, yielding CPU
Process [60384]: 2.00, yielding CPU
Process [60385]: 2.00, yielding CPU
Process [60384]: 2.25, yielding CPU
Process [60385]: 2.25, yielding CPU
Process [60384]: 2.50, yielding CPU
Process [60385]: 2.50, yielding CPU
Process [60384]: 2.75, yielding CPU
Process [60385]: 2.75, yielding CPU
Process [60384]: 3.00, yielding CPU
Process [60385]: 3.00, yielding CPU
Process [60384]: Done
Process [60385]: Done
```

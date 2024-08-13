# Exercise 55-2

Write a program to determine whether *flock()* detects deadlock situations when
being used to lock two different files in two processes.

## Solution

I designed a program where process begins by forking a child process.

1. The parent acquires an exclusive lock on file 1, lets the child process know that it can
proceed. It then blocks waiting for the child to allow it to resume.

2. Similarly, the child acquires an exclusive lock on file 2, and blocks until the parent tells
it to proceed before resuming.

3. When notified by child, the parent attempts to lock file 2, blocking up to a timeout.

4. The child does the same but for file 1.

I used a pair of unnamed POSIX semaphores for synchronization of the related processes, 
available to the related processes via a shared anonymous mapping.

Since the semantics of *flock()* are such that the lock is associated with the open file
description rather than the i-node or the file descriptor (see page 1122), I was careful to
open the files to be locked *after* the *fork()*; if done before, the child would inherit
a pair of file descriptors pointing to the same file description, and the two processes
would share locks.

I decided that if a deadlock situation was detected, then either call would fail immediately.
Therefore, I decided to introduce a timeout of 5 seconds by using the *alarm()* system call.
If after the timeout the child has not been able to acquire both locks, then I presume
a deadlock has occurred, and cause the child to exit. At that point, the lock on file 2
held by the child is released, allowing the parent process to acquire the lock.

To compile and run:

```bash
# pthread required for POSIX semaphores
 gcc -Wall -o ./flock_deadlock flock_deadlock.c -pthread
 ./flock_deadlock README.md flock_deadlock.c
```


I observed the following output;

```
Parent has lock on README.md, waiting to acquire lock on flock_deadlock.c
Child owns lock on flock_deadlock.c, waiting to acquire lock on README.md
Child requested to wait for lock timed out, exiting
Parent acquired lock on flock_deadlock.c, waiting to reap child
```

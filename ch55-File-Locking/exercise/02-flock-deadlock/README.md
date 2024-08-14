# Exercise 55-2

Write a program to determine whether *flock()* detects deadlock situations when
being used to lock two different files in two processes.

## Solution

I designed a program in which a process begins by forking a child. I then used two unnamed
POSIX semaphores to synchronize the actions of the two related process:

1. The parent acquires an exclusive lock on file 1, then posts to the child's semaphore so
that it may resume. The parent then waits on the child to post to its semaphore. 

2. Similarly, the child acquires an exclusive lock on file 2, posts to the parent's semaphore,
and waits for the parent to post to its semaphore.

3. When the parent acquires the semaphore, it attempts to lock file 2.

4. Similarly, when the child acquires the semaphore, it attempts to lock file 1. However,
since I suspected that a deadlock may not be detected, I introduced a timeout by using
the *alarm()* system call and handling the `SIGALRM` signal (whose default action would
otherwise terminate the child process).

Since the semantics of *flock()* are such that the lock is associated with the open file
description rather than the i-node or the file descriptor (see page 1122), I was careful to
open the files to be locked *after* the *fork()*; if done before, the child would inherit
a pair of file descriptors pointing to the same file description, and the two processes
would share locks.

The timeout I introduced on the child was 5 seconsd. I decided that if the process did not
terminate before the timeout, then a deadlock must have occurred. I confirmed my observation
by running the program. No output was shown in stdout for 5 seconds, and at that point, the child
outputs a message saying that *flock()* failed to due to `EINTR`. This means that the signal
handler introduced for the `SIGALRM` signal interrupted *flock()* on the child while it blocked
waiting for an exclusive lock on file 1. Given that there was no prior output from the parent
either, I deduced a deadlock occurred.

At that point, the child exits, and the lock on file 2 held by the child is released, allowing
the parent process to acquire the lock.

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

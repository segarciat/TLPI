# Exercise 63-7

Suppose we have an *epoll* file descriptor that is monitoring multiple file descriptors,
all of which are always ready. If we perform a series of *epoll_wait()* calls in which
*maxevents* is much smaller than the number of ready file descriptors (e.g., *maxevents*
is1), without performing all possible I/O ont he ready descriptors between calls, what
descriptor(s) does *epoll_wait()* return in each call? Write a program to determine
the answer. (For the purposes of this experiment, it suffices to perform no I/O between
the *epoll_wait()* system calls.) Why migiht this behavior be useful?

## Solution

I implemented a program similar to Listing 63-2, which creates the number of pipes specified
in the first command-line argument. The program writes a single byte to the write end
of each pipe, and then adds the file descriptor corresponding to the pipe's read end
to the *epoll* interest list. It then enters a loop where it performs a fixed number of
polls, each for at most 1 file descriptor. The number of polls can be specified as a
second command-line argument, but it defaults to 1 million otherwise. The program does
not read the data in the pipe, so all of the read file descriptors for the pipes
are always ready.

I ran the program together with the `sort` and `uniq -c` commands to determine how many
times each file descriptor was polled.

To build and run:

```bash
make
./epoll_always_ready 7 | sort | uniq -c
```

The output I observed was:

```
 142857 ready: fd 10
 142857 ready: fd 12
 142857 ready: fd 14
 142857 ready: fd 16
 142858 ready: fd 4
 142857 ready: fd 6
 142857 ready: fd 8
```

It appears that the kernel employs a round-robin-like approach when returning a file descriptor
from the ready list. This could be useful to ensure that no file descriptor is starved
of I/O.

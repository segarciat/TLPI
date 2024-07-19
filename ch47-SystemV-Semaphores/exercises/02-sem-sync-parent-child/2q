# Exercise 47.2

Modify the program in Listing 24-6 (`fork_sig_sync.c`, on page 528) to use semaphores instead
of signals to synchronize the parent and child processes.

##  Solution

The original program, `fork_sig_sync.c`, was programmed to use a `SIGUSR1` signal and
`sigsuspend()` to have the parent wait on the child, and the child issued the signal
using the `kill()` system call. In my implementation, I created a semaphore set with
a single semaphore whose value is initialized to 0, prior to using `fork()` to create
the child process. Semantically, this means the semaphore is reserved by the child.
The parent tries to decrease the semaphore and it blocks because a semaphore's value
cannot fall below 0. When the child is done, it increases the semaphore to 1, which
then allows the parent to proceed.

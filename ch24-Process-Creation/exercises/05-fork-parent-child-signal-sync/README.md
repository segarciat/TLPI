# Exercise 24.5

Suppose that in the program in Listing 24-6, the child process also needed to
wait on the parent to complete some actions. What changes to the program would
be required in order to enforce this?

## Solution

In the child, after sending the initial synchronization signal to the parent,
the child can use `sigsuspend()` or `sigwaitinfo()` to wait for the signal from
the parent. Since the synchronization signal is added to the process signal mask
before the `fork()`, it will become pending when it is generated. In the parent
after completing the actions of interest, we use `kill(childPid, SYNC_SIG)` to
send the signal to the child, optinally followed by `wait(NULL)` or (`wait(&status)`
if the parent cares about the termination status of the child). Finally, the child
can `_exit()`.

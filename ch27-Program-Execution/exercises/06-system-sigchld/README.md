# Exercise 27.6

Suppose that a parent process has established a handler for `SIGCHLD` and also blocked this
signal. Subsequently, one of its children exits, and the parent does a `wait()` to collect
the child's status. What happens when the parent unblocks `SIGCHLD`? Write a program to
verify your answer. What is the relevance of the result for a program calling the `system()`
function?

## Solution

The `wait()` system function returns when the child terminates even if the parent process
is blocking the signal. When the child terminates, it is reaped by the parent due to the
`wait()` system call, but it also causes a `SIGCHLD` to be delivered to the parent. Since
the parent has blocked the signal, it is added to the set of pending signals for the process;
when the parent unblocks it, it receives the signal, and it is caught by the handler.

I wrote a simple program called `waited_and_handler_invoked.c` to verify this:

```bash
gcc waited_and_handler_invoked.c
./a.out
```

The output was as expected:

```
Blocked SIGCHLD, which current is not in the set of pending signals
SIGCHLD handler installed in parent
Parent waiting on child...
Forked! Child (36173) exiting...
Parent has reaped child
SIGCHLD is still blocked, and it is in the set of pending signals. Unblocking
SIGCHLD handler was invoked
```

Since `system()` creates its own child and reaps it, this means that a program calling `system()`
needs to be wary of this fact. Otherwise, it may reap a child that it did not directly create,
and which has already exited. The `wait()` system call returns `ECHILD` in such a case, so the
calling program should check for this error code to decide if it should resume normal operation.

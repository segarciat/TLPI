# Exercise 33.2

Suppos that a thread creates a child using `fork()`. When the child terminates, is it guaranteed
that the resulting `SIGCHILD` will be delivered to the thread that called `fork()` (as opposed
to some other thread in the process)?

## Solution

No. According to Section 33.2.1, a signal is thread-directed if one of the following is true:

- It is generated as the direct result of the execution of a specific hardware instruction
within the context of the thread.

- It is a `SIGPIPE` generated when the thread tried to write to a broken pipe.

- It is sent using `pthread_kill()` or `pthread_queue()`.

As described in Section 2.6.3.1, a `SIGCHLD` is sent to a parent process whenever one of its children
terminates. The parent process is identified by the process ID, and all threads share the same
process ID. Guaranteeing delivery to the thread that called `fork()` violates the requirement that
the kernel should arbitrarily select one thread in the process to which to deliver a signal and
invoke the handler in that thread.

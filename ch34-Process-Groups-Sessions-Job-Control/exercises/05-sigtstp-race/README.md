# Exercise 34.5

Suppose that, in the signal handler of Listing 34.6, the code that unblocks the
`SIGTSTP` signal was moved to the start of the handler. What potential race condition
does this create?

## Solution

Since the signal handler was installed to handle `SIGTSTP`, the signal is automatically
added to the process signal mask prior to its invocation, causing the signal's delivery
to be blocked. 

Suppose the `sigprocmask()` call that unblocks the signal is moved so that it's sandwiched
between the `signal()` call that resets the disposition of `SIGTSTP` to `SIG_DFL` and the
call `raise(SIGTSTP)`. Then if the user types the *suspend* (CTRL+Z) before `raise()` runs,
then the process will be stopped. When it is resumed, `raise()` will stop it again,
which means that `SIGCONT` needs to be sent again to resume the process.

Worse, if `sigprocmask()` is moved before the `signal()` call that resets the disposition
to `SIG_DFL`, then the program is subject to a stack overflow. This can happen because
a process may send `SIGTSTP` signals to this process at a very fast rate before the
signal handler has a chance to call `signal()`. The result is that the signal handler
recursively interrupts itself.

# Exercise 20.3

Write programs that verify the effect of the `SA_RESETHAND` and `SA_NODEFER` flags when establishing
a signal handler with `sigaction()`.

## Solution

The `SA_RESETHAND` is a flag that can be specified in the `sa_flags` argument of a `struct sigaction`
object when calling the `sigaction()` system call to change the disposition of a process for a
specified signal. As described on page 417:

*When the signal is caught, the disposition is reset to the default (`SIG_DFL`)
before invoking the handler. The default behavior otherwise is that the signal handler remains
established until it is explicily disestablished by another call to `sigaction()`.*

The `SA_NODEFER` flag makes it so that when the flag is caught, it is not automatically added
to the process signal mask while the handler is executing. The default is precisely the opposite;
the signal would be automatically added to the signal mask. As described in Section 20.13, this
is to ensure that a *signal handler won't recursively interrupt itself if a second instance of
the same signal arrives while the handler executes*.

In my sample program, the signal in question is *SIGINT* which can be issued with *CTRL+C* or
the *kill(1)* command. Messages are printed by making calls to functions in `<stdio.h>`,
which is deemed unsafe as described in chapter 20, but it's ok for these simple verifications.

```bash
gcc sigaction_sa_flags.c
# Press CTRL+C as instructed
```

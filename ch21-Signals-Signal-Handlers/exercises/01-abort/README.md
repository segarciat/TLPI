# Exercise 21.1

Implement `abort()`.

## Solution

The `abort()` function, part of `stdlib.h`, terminates the calling process, causing it to produce
a core dump. It does so by raising a `SIGABRT` signal. According to page 433 on Section 21.2.2,
SUSv3 additionally specifies that `abort()` must override the effect of blocking or ignoring `SIGABRT`.
Furthermore, SUSv3 specifies that `abort()` must terminate the process unless the process catches the
signal with a handler that doesn't return. Kerrisk goes on to suggest that if a handler catches the
signal and returns, then the `abort()` function resets the handling of `SIGABRT` to `SIG_DFL`
and raises a second `SIGABRT`. Also, when it successfully terminates the process, it also flushes
and closes *stdio* streams.

I used `sigprocmask()` to ensure `SIGABRT` was removed from the process signal mask. Then I used
`sigaction()` to obtain the current disposition for `SIGABRT`. In the scenario that it was neither
`SIG_DFL` nor `SIG_IGN`, I assumed then that it was a programmer-defined handler, which I allowed
to be called. Upon return (if the handler did not have a non-local goto/long jump), I reset the
handler to `SIG_DFL`, flushed all *stdio* streams with `fflush()`, and closed `stdin`, `stdout`,
and `stderr` before issuing `SIGABRT` once more with `raise()`.

# Exercise 20.4

Implement the `siginterrupt()` function described in Section 21.5 using `sigaction()`.

## Solution

The `siginterrupt()` function, described on Section 21.5 (page 445) changes the `SA_RESTART`
setting associated with a singla. It accepts two integer arguments. The first is a *signal*,
and the second one is a *flag* that should be 0 or 1. If true (1), then a handler for the signal
will interrupt blocking system calls. If false (0), the blocking system call will be restarted
after execution of the handler.

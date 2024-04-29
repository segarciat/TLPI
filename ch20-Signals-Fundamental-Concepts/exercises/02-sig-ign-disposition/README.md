# Exercise 20.2

Write a program that shows that when the disposition of a pending signal is changed to
be `SIG_IGN`, the program never sees (catches) the signal.

## Solution

My program uses calls `sigaction()` with a `SIGINT` as the `sig` argument to change the
disposition of the program when it is delivered *SIGINT* a signal. For the `sigaction` argument,
I specified a `struct sigaction` structure whose `sa_handler` field is `SIG_IGN`. Then,
I used `sleep()` to make the process idle for a short duration to allow me time to
try to attempt to terminate the process with a *CTRL+C* (a *SIGINT*). After the sleep
is over, I reset the disposition to the default with `SIG_DFL` and inform the user
that the progress will run until and sleep until a terminating signal is received.

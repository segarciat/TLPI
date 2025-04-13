# Exercise 20.2

Write a program that shows that when the disposition of a pending signal is changed to
be `SIG_IGN`, the program never sees (catches) the signal.

## Solution

My program uses calls `sigaction()` with a `SIGINT` as the `sig` argument to change the
disposition of the program when it is delivered *SIGINT* a signal. For the `sigaction` argument,
I specified a `struct sigaction` structure whose `sa_handler` field is `SIG_IGN`. Then,
I used `sleep()` to make the process idle for a short duration to allow me time to
attempt to terminate the process with a *CTRL+C* (a *SIGINT*). After the sleep
is over, I displayed the pending signal set, which I expected to be empty. Then
I reset the disposition to the default with `SIG_DFL` and inform the user
that the process will sleep until a terminating signal is received (such as `SIGINT`).

Here is a sample run:

```bash
make
./sig_ign_disposition
```

Here is the sample interaction:

```
Interrupt ignored. Sleeping for about 5 seconds...
^C^C^C^C^C^C^C^C^C^C^C^C^C^CDone sleeping! Checking signals in pending set.
Pending signals:
		<empty signal set>
Resetting disposition of Interrupt
Pausing until interrupted... (Press CTRL+C)
^C
```

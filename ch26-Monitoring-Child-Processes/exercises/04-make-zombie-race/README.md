# Exercise 26.4

Listing 26.4 (`make_zombie.c`) uses a call to *sleep()* to allow the child process a chance to execute
and terminate before the parent executes *system()*. This approach produces a theoretical race condition.
Modify the program to eliminate the race condition by using signals to synchronize the parent and
child.

## Solution

The theoretical race condition is that in spite of the 3 seconds sleep in the parent, the child is not
guaranteed to have terminated. Since a `SIGCHLD` is sent to a parent process when a child terminates,
we can install a handler that suspends until it is received.

I first blocked `SIGCHLD`:

```c
	/* Block SIGCHLD before sigsuspend */
	sigset_t blockMask;
	sigemptyset(&blockMask);
	sigaddset(&blockMask, SIGCHLD);
	if (sigprocmask(SIG_SETMASK, &blockMask, NULL) == -1)
		errExit("sigprocmask");
```

Then I replaced `sleep(3)` with `sigwaitinfo()` to wait for `SIGCHLD`:

```c
		sigwaitinfo(&blockMask, NULL); /* Give child a chance to start and exit */
```

If we don't block the signal prior to `sigwaitinfo()`, then the child could terminate before the
parent is scheduled, and the parent suspends forever. Moreover, we cannot block after the `fork()`
for the same reason.

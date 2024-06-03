# Exercise 34.4

Modify the program in Listing 3.4 (`disc_SIGHUP.c`) to verify that, if the controlling
process doesn't terminate as a consequence of receiving `SIGHUP` (i.e, it instead ignores
or catches the signal and continues execution), then the kernel doesn't send `SIGHUP`
to the members of the foreground process group.

## Solution

I modified the program so that the parent, which is also the controlling process, installs
the same `SIGHUP` signal handler as the children. The parent suspends with `sigwaitinfo()`
in order to synchrnously accept the `SIGRTMIN` signal. It uses a loop to accept the same
number of signals as children created by the parent. Each child uses `sigqueue` to queue
`SIGRTMIN`. Then the parent uses `raise(SIGHUP)` to deliver itself a signal. Then the
program continues just like before.

Below is a snippet showing the relevant parts I made changes to:
```c
	sigset_t blockMask;
	sigemptyset(&blockMask);
	sigaddset(&blockMask, SIGRTMIN);
	sigprocmask(SIG_BLOCK, &blockMask, NULL);
    for (j = 1; j < argc; j++) {        /* Create child processes */
			/* Other existing code */
												/* Child notifies parent */
			if (sigqueue(parentPid, SIGRTMIN, (const union sigval) 0) == -1)
				errExit("sigqueue");
            break;                      /* Child exits loop */
        }
    }

    /* All processes fall through to here */
    alarm(60);      /* Ensure each process eventually terminates */

	if (parentPid == getpid()) {		/* Only parent proceeds here */
    	printf("Parent: PID=%ld PGID=%ld\n", (long) getpid(), (long) getpgrp());
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = 0;
		sa.sa_handler = handler;
		if (sigaction(SIGHUP, &sa, NULL) == -1)
			errExit("sigaction parent");
		printf("Suspending until all children have sent SIGRTMIN\n");
		for (int i = 1; i < argc; i++)
			sigwaitinfo(&blockMask, NULL);
		printf("Parent raising SIGHUP\n");
		raise(SIGHUP);
		printf("Parent continuing\n");
	}

	for (;;)
		pause();        /* Wait for signals */
```

The program can be compiled and run as follows

```bash
make
exec ./disc_SIGHUP d s s > non_term_sig.log 2>&1
```

As described in the book, the `exec` command is a shell built-in that causes the shell to do
an *exec()*, replacing itself with the named program. The output of the program, including
errors, is redirected to `non_term_sig.log`. Running `cat non_term_sig.log` from a different
terminal shows the following:

```
PID of parent process is:       22876
Foreground process group ID is: 22876
PID=23109 PGID=23109
Parent: PID=22876 PGID=22876
PID=23110 PGID=22876
Suspending until all children have sent SIGRTMIN
PID=23111 PGID=22876
Parent raising SIGHUP
PID 22876: caught signal  1 (Hangup)
Parent continuing
```

The parent process handled the `SIGHUP` signal, and it continued without terminating.
None of the children handled the signal because it was not delivered to them; it would been
delivered had the parent process terminated as a result. Allowing the process to terminate
(which will automatically happen after 60 seconds due to `alarm(60)`) shoes that no
more lines of output have been added.
the parent did not terminate as a result.

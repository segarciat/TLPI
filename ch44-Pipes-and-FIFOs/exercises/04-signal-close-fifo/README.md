# Exercise 44.4

Add code to the server in Listing 44-7 (`fifo_seqnum_server.c`) so that if the program
receives the `SIGINT` or `SIGTERM` signal, it removes the server FIFO and terminates.

## Solution

The starting code I used is my solution to Exercise 44.3, where I extended `fifo_seqnum_server.c`
to use a backup file for the sequence number.

The handler for `SIGTERM` and `SIGINT` is simple; it uses the `unlink()` system call to remove
the server fifo, whose name is declared in the `fifo_seqnum.h` header :

```c
/* SIGINT and SIGTERM handler: deletes FIFO and terminates */
void
signalHandler(int sig)
{
	if (unlink(SERVER_FIFO) == -1)
		_exit(EXIT_FAILURE);
	_exit(EXIT_SUCCESS);
}
```

In main, I proceed to install the handler before creating the FIFO:

```c
int
main(int argc, char *argv[])
{
	/* More code above here */
	sigset_t blockSigMask, prevSigMask;
	struct sigaction sigactionHandler;

    umask(0);                           /* So we get the permissions we want */

	/* Other code in between here */

	/* Block signals prior to installing handler */
	sigemptyset(&blockSigMask);
	sigaddset(&blockSigMask, SIGINT);
	sigaddset(&blockSigMask, SIGTERM);
	if (sigprocmask(SIG_BLOCK, &blockSigMask, &prevSigMask) == -1)
		errExit("sigprocmask: failed to add SIGINT and SIGTERM to signal mask");
	
	/* Install signal handler */
	sigactionHandler.sa_flags = 0;
	sigemptyset(&sigactionHandler.sa_mask);
	sigactionHandler.sa_handler = signalHandler;
	if (sigaction(SIGINT, &sigactionHandler, NULL) == -1)
		errExit("sigaction SIGINT");
	if (sigaction(SIGTERM, &sigactionHandler, NULL) == -1)
		errExit("sigaction SIGTERM");
```

Notice that I block `SIGINT` and `SIGTERM` prior to installing the handler to avoid a race
condition that would occur if the signal handler were called before the FIFO was created.
If we attempted to install the signal handler after having created the FIFO, then we still
have a race condition because the server could be signaled before installing the handler,
and the FIFO would not be deleted.

I proceed to unblock the signals after creating the FIFO, at which point any pending `SIGTERM`
or `SIGINT` signals may be delivered:

```c

	/* Create well-known FIFO, and open it for reading */
	if (mkfifo(SERVER_FIFO, S_IRUSR | S_IWUSR | S_IWGRP) == -1
            && errno != EEXIST)
        errExit("mkfifo %s", SERVER_FIFO);

	/* Unblock signal SIGINT and SIGTERM */
	if (sigprocmask(SIG_SETMASK, &prevSigMask, NULL) == -1)
		errExit("sigprocmask: failed to restore signal mask");
	/* More code below */
```

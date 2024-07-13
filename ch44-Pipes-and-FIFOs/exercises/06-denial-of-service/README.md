# Exercise 44.6

The server in Listing 44-7 (`fifo_seqnum_server.c`) assumes that the client process is well behaved.
If a misbehaving client created a client FIFO and sent a request to the server, but did not open its
FIFO, then the server's attempt to open the client FIFO would block, and other client's requests would
be indefinitely delayed. (If done maliciously, this would constitute a *denial-of-service-attack*.)
Devise a scheme to deal with this problem. Extend the server (and possibly the client in Listing 44-8)
accordingly.

## Solution

One idea is to use a timer or alarm in the server process prior to attempting the open
the client FIFO. This schedules a signal to be generated and delivered to the process
in the future, whose default action is to interrupt any blocking system call (in this
case *open()*) and then kills the process. By installing a signal handler, we can arrange
for a global variable to be set indicating whether the server's attempt to open the client
FIFO timed out, and keep the server process alive. Then the server can proceed to service
the next client. A downside of this approach is that, although the server will not block
indefinitely, the clients that are behaving correctly are now subject to the delay which
incurred by the alarm timeout.

For example, we could define a handler that does nothing:

```c
#define CLIENT_OPEN_TIMEOUT 1

static void
alarmSignalHandler(int sig)
{
}
```

Then, in the main loop of the server's `main()`:

```c
int
main(int argc, char *argv[])
{
	/* install handler */
	struct sigaction sa;
	sa.flags = 0;
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = alarmSignalHandler;
	if (sigaction(SIGALRM, &sa, NULL) == -1)
		errExit("sigaction: failed to install alarm handler");
	for (;;) {
		/* in main loop, attempt set alarm and then attempt to open client FIFO */
		alarm(CLIENT_OPEN_TIMEOUT);
		clientFd = open(clientFifo, O_WRONLY);
		if (clientFd == -1 && errno == EINTR) {
			errMsg("open %s", clientFifo);
			if (errno == EINTR)
				errMsg("timed out");
			continue;
		}
		/* rest of loop */
	}
	/* rest of main() */
}
```

The server could alternatively provide the `O_NONBLOCK` flag to the *open()* system call
when attempting to open the client's FIFO. According to Section 5.9, this normally causes
*open()* to return an error instead of blocking if the call cannot succeed immediately.
Section 44.9 goes on to elaborate on this by saying that if the FIFO is being opened for
writing (which is true in the case of the server), and the other end of the FIFO is not
already open for reading, then *open()* fails, setting `errno` to `ENXIO`. Thus, this change
alone is not enough, because it's possible that a valid client may write to the server FIFO
and is then preempted by the kernel before it is able to open its own FIFO. The result is
that the server would interpret this as a misbehaving client, therefore abandoning a valid
request. For this approach to work, the client must also be modified by opening its own FIFO
before sending a request to the server. We might expect this to cause an error because the FIFO
is not yet open for writing (and it will never be in this approach), and that this would
cause a deadlock. However, according to Section 44.9, the semantics of the `O_NONBLOCK` flag
given to *open()* differ when opening a FIFO for reading. If the FIFO is being opened for reading
(in the case of the client) and the other end of the FIFO is not already open for reading, then
*open()* succeeds.

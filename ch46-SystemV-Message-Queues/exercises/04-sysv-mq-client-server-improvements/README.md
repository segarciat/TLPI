# Exercise 46.4

Make the following changes to the client-server application of Section 46.8:

a) Replace the use of a hard-coded message queue key with code in the server
that uses `IPC_PRIVATE` to generate a unique identifier, and then writes this
identifier to a well-known file. The client must read the identifier from this
file. The server should remove this file when it terminates.

b) In the *serveRequest()* function of the server program, system call errors
are not diagnosed. Add code that logs errors using *syslog()* (Section 37.5).

c) Add code to the server so that it becomes a daemon on startup (Section 37.2)

d) In the server, add a handler for `SIGTERM` and a `SIGINT` that performs a tidy
exit. The handler should remove the message queue and (if the earlier part of this
exercise was implemented) the file created to hold the server's message queue
identifier. Include code in the handler to terminate the server by disestablishing
the handler, and then once more raising the same signal that invoked hte handler
(see Section 26.1.4 for the rational and steps for this task).

e) The server child doesn't handle the possibility that the client may terminate
prematurely, in which case the server child would fill the client's message queue,
and then block indefinitely. Modify the server to handle this possibility by
establishing a timeout when calling *msgsnd()*, as described in Section 23.3. If
the server child deems that the client has disappeared, it should attempt to
delete the client's message queue, and then exit (after perhaps logging a message
via *syslog()*.

## Solution

a) I added the following macro definition to the header file:

```c
#define SERVER_MQID_FILE "./server_mqid"
```

In the server file, I created the file as follows:

```c
#define MSQID_MAX_LEN 100
	char msqIdStr[MSQID_MAX_LEN];

    /* Create server message queue */

    serverId = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL |
                            S_IRUSR | S_IWUSR | S_IWGRP);
	if (serverId == -1)
        errExit("msgget");

	/* To ensure we get the permissions we want */
	umask(0);

	/* Write identifier to "well-known file" */
	fd = open(SERVER_MQID_FILE, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP);
	if (fd == -1)
		errExit("open() - failed to create message queue ID file");
	snprintf(msqIdStr, MSQID_MAX_LEN, "%d", serverId);
	if (write(fd, msqIdStr, strlen(msqIdStr)) != strlen(msqIdStr))
		errExit("write() - failed to write message queue ID to file");
```

As instructed, I employ the  `IPC_PRIVATE` key in *msgget()*. I use `umask(0)` prior
to making the file, to ensure I get the permissions I want (read-write for owner which
is the server, and read for group, which a client will be part of). In the client,
I defined a corresponding function *getServerMessageQueueId()* that reads the ID from
this file.

b) I began by establishing a connection with the system log facility and sets defaults
for subsequent *syslog()* calls by using *openlog()*:

```c
	openlog(argv[0], LOG_PID | LOG_CONS | LOG_NOWAIT, LOG_USER);
```

The `LOG_PID` option ensures that the PID of the child forked to serve the request is
logged with each message. The `LOG_CONS` causes the message to be written to the system
console if there is an error sending it to the system logger. The `LOG_NOWAIT` ensures
that no *wait()* is performed for any child process that may have been created in order
to log the message. The `LOG_USER` is provided as the facility, which just says that
a user process generated the message.

The following is one of the calls I added:

```c
		syslog(LOG_ERR, "Couldn't open %s for client %d: %m", req->pathname, req->clientId);
```

*syslog()* replaces the 2-character sequence `%m` with the error string corresponding to
the current vlaue of *errno*, the equivalent of *strerror(errno)*.

I tried this by running the client with a nonexistent file, then verified the system log
file:

```bash
make svmsg_file_server
make svmsg_file_client
./svmsg_file_server &
# No such file fooe xists
./svmsg_file_client foo
tail /var/log/syslog
```

The server outputs `Couldn't open file` from the background, and the system log file shows:

```
Jul 18 00:35:28 sgarciat-TUF-FX505GT ./svmsg_file_server[10261]: Couldn't open foo for client 6: No such file or directory
```

c) As discussed in 37.2, there are several steps that a client must take to become a daemon.
Among them, the process must belong to its own session and not be a session leader, thereby freeing
it from any controlling terminal. Moreover, the process umaski s cleared, its current directory is
changed to the root of the filesystem, all open file descriptors are closed, and the standard file
descriptors are referred to `/dev/null`. This is a rough summary of the necessary steps, detailed
in Section 37.2, and handled by the nonstandard *daemon()* function provided by the GNU C library.

d) I began by installing the same handler for both signals:

```c
	/* Establish SIGTERM and SIGINT handler to remove queue before terminating */
	sigemptyset(&sigTermIntsa.sa_mask);
	sigaddset(&sigTermIntsa.sa_mask, SIGTERM);
	sigaddset(&sigTermIntsa.sa_mask, SIGINT);
	sigTermIntsa.sa_flags = SA_RESTART;
	sigTermIntsa.sa_handler =  removeQueue;
	if (sigaction(SIGTERM, &sigTermIntsa, NULL) == -1)
		errExit("sigaction - SIGTERM");
	if (sigaction(SIGINT, &sigTermIntsa, NULL) == -1)
		errExit("sigaction - SIGINT");
```

I made sure to add both `SIGTERM` and `SIGINT` to the signal mask upon execution of the handlers
so that neither can recursively interrupt the other while handling a signal. The handler itself
is simple:

```c
static void				/* SIGTERM and SIGINT handler */
removeQueue(int sig)
{
	/* Remove message queue */
	if (msgctl(serverId, IPC_RMID, NULL) == -1)
		syslog(LOG_ERR, "Failed to remove message queue with ID %d after signal %d was caught: %m",
							serverId, sig);
	/* Remove message ID file */
	if (unlink(SERVER_MQID_FILE) == -1)
		syslog(LOG_ERR, "Failed to remove message queue ID file %s after signal %d as caught: %m",
							SERVER_MQID_FILE, sig);
	/* Uninstall handler and raise signal again */
	signal(sig, SIG_DFL);
	raise(sig);
}
```

I write to the system log file if there are any errors while attempting to remove the queue
or the file. The following can be used to verify it works properly:

```bash
make svmsg_file_server
# Control returns to user since daemon does not have a controlling terminal
./svmsg_file_server
# Find out PID of server: 14119
ps xa | grep sysmsg_file_server
# Verify a queue was created
ipcs
# Verify the ID file was created in the current directory
ls
# Kill the process
kill -SIGTERM 14119
# Verify process is dead, and that queue and ID file are gone
ps xa | grep sysmv_file_server
ipcs
ls
```

e) I employed the *alarm()* system call to allow a 3 second timeout. Moreover, to test, I extended
the client program to accept a second optional argument, which satisfies the value of the *msg_qbytes*
field of the *struct msqid_ds* structure corresponding to the client's message queue. I employed this
by setting it to 0, so that the upper limit on the number of bytes is 0:


```bash
make svmsg_file_client
./svmsg_file_client README.md 0
```

This causes the *msgsnd()* call in the child process to block, and since the *alarm()* all interrupts
it after 3 seconds, the child goes on to clean up. The following can be seen in the system log file
after this occurs:

```
Jul 18 01:59:39 sgarciat-TUF-FX505GT ./svmsg_file_server[16921]: Timed out while waiting to send message to client 17: Interrupted system call
Jul 18 01:59:39 sgarciat-TUF-FX505GT ./svmsg_file_server[16921]: Successfully deleted client message queue with id 17
```

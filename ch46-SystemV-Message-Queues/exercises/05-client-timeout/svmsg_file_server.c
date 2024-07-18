/* Modified by Sergio E. Garcia Tapia for Exercise 46.4 */

/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2023.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 46-8 */

/* svmsg_file_server.c

   A file server that uses System V message queues to handle client requests
   (see svmsg_file_client.c). The client sends an initial request containing
   the name of the desired file, and the identifier of the message queue to be
   used to send the file contents back to the child. The server attempts to
   open the desired file. If the file cannot be opened, a failure response is
   sent to the client, otherwise the contents of the requested file are sent
   in a series of messages.

   This application makes use of multiple message queues. The server maintains
   a queue (with a well-known key) dedicated to incoming client requests. Each
   client creates its own private queue, which is used to pass response
   messages from the server back to the client.

   This program operates as a concurrent server, forking a new child process to
   handle each client request while the parent waits for further client requests.
*/
#include "svmsg_file.h"
#include <syslog.h> /* syslog() */

#define MQ_SND_TIMEOUT 3

static int serverId;

static void             /* SIGCHLD handler */
grimReaper(int sig)
{
    int savedErrno;

    savedErrno = errno;                 /* waitpid() might change 'errno' */
    while (waitpid(-1, NULL, WNOHANG) > 0)
        continue;
    errno = savedErrno;
}

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

static void
msgSendTimeout(int sig)	/* SIGALARM handler */
{
}

static void             /* Executed in child process: serve a single client */
serveRequest(const struct requestMsg *req)
{
    int fd, savedErrno;
    ssize_t numRead;
    struct responseMsg resp;

	errno =  0;
    fd = open(req->pathname, O_RDONLY);
    if (fd == -1) {                     /* Open failed: send error text (%m is value of errno) */
		syslog(LOG_ERR, "Couldn't open %s for client %d: %m", req->pathname, req->clientId);
        resp.mtype = RESP_MT_FAILURE;
        snprintf(resp.data, sizeof(resp.data), "%s", "Couldn't open");
        msgsnd(req->clientId, &resp, strlen(resp.data) + 1, 0);
        exit(EXIT_FAILURE);             /* and terminate */
    }

    /* Transmit file contents in messages with type RESP_MT_DATA. We don't
       diagnose read() and msgsnd() errors since we can't notify client. */

    resp.mtype = RESP_MT_DATA;
	errno = 0;
	while ((numRead = read(fd, resp.data, RESP_MSG_SIZE)) > 0) {
		alarm(MQ_SND_TIMEOUT);
		int result = msgsnd(req->clientId, &resp, numRead, 0);
		savedErrno = errno;
		alarm(0);	/* Disable alarm */ 
		errno = savedErrno;
		if (result == -1) {
			if (errno == EINTR) {
				syslog(LOG_ERR, "Timed out while waiting to send message to client %d: %m",
									req->clientId);
				if (msgctl(req->clientId, IPC_RMID, NULL) == -1)
					syslog(LOG_ERR, "Failed to delete client message queue with id %d: %m",
						req->clientId);
				else
					syslog(LOG_INFO, "Successfully deleted client message queue with id %d", req->clientId);
				exit(EXIT_FAILURE);
			} else {
				break;
			}
		}
	}
	
	if (errno != 0)
		syslog(LOG_ERR, "Error sending contents of %s to client %d: %m", req->pathname, req->clientId);

    /* Send a message of type RESP_MT_END to signify end-of-file */

    resp.mtype = RESP_MT_END;
    msgsnd(req->clientId, &resp, 0, 0);         /* Zero-length mtext */
}

int
main(int argc, char *argv[])
{
    struct requestMsg req;
    pid_t pid;
    ssize_t msgLen;
    struct sigaction sigChldsa, sigTermIntsa, sigAlrmsa;
	int fd;
#define MSQID_MAX_LEN 100
	char msqIdStr[MSQID_MAX_LEN];

	/* Becomea daemon */
	if (daemon(/* Don't change current directory */ 1, /* redirect std descriptors */ 0) == -1)
		errExit("daemon() - Failed to become a daemon");

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

    /* Establish SIGCHLD handler to reap terminated children */

    sigemptyset(&sigChldsa.sa_mask);
    sigChldsa.sa_flags = SA_RESTART;
    sigChldsa.sa_handler = grimReaper;
	if (sigaction(SIGCHLD, &sigChldsa, NULL) == -1)
		errExit("sigaction - SIGCHLD");

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

	/* Establish SIGALRM handler for message send timeouts */
	sigemptyset(&sigAlrmsa.sa_mask);
	sigAlrmsa.sa_flags = 0;
	sigAlrmsa.sa_handler = msgSendTimeout;
	if (sigaction(SIGALRM, &sigAlrmsa, NULL) == -1)
		errExit("sigaction - SIGALRM");

	openlog(argv[0], LOG_PID | LOG_CONS | LOG_NOWAIT, LOG_USER);

    /* Read requests, handle each in a separate child process */

    for (;;) {
        msgLen = msgrcv(serverId, &req, REQ_MSG_SIZE, 0, 0);
        if (msgLen == -1) {
            if (errno == EINTR)         /* Interrupted by SIGCHLD handler? */
                continue;               /* ... then restart msgrcv() */
            errMsg("msgrcv");           /* Some other error */
            break;                      /* ... so terminate loop */
        }

        pid = fork();                   /* Create child process */
        if (pid == -1) {
            errMsg("fork");
            break;
        }

        if (pid == 0) {                 /* Child handles request */
            serveRequest(&req);
            _exit(EXIT_SUCCESS);
        }

        /* Parent loops to receive next client request */
    }

    /* If msgrcv() or fork() fails, remove server MQ and exit */

    if (msgctl(serverId, IPC_RMID, NULL) == -1)
        errExit("msgctl");
    exit(EXIT_SUCCESS);
}

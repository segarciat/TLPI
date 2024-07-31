/* Modified by Sergio E. Garcia Tapia for Exercise 52-3 */

/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2023.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Exercise 52-3 (based on Listing 46-8) */

/* pmsg_file_server.c

   A file server that uses POSIX message queues to handle client requests
   (see pmsg_file_client.c). The client sends an initial request containing
   the name of the desired file, and the identifier of the message queue to be
   used to send the file contents back to the child. The server attempts to
   open the desired file. If the file cannot be opened, a failure response is
   sent to the client, otherwise the contents of the requested file are sent
   in a series of messages.

   This application makes use of multiple message queues. The server maintains
   a queue (with a well-known nmae) dedicated to incoming client requests. Each
   client creates its own private queue, which is used to pass response
   messages from the server back to the client.

   This program operates as a concurrent server, forking a new child process to
   handle each client request while the parent waits for further client requests.
*/
#include "pmsg_file.h"
#include <sys/wait.h>

#define SERVER_MQ_MAXMSG 10
#define SERVER_MQ_MSGSZ  sizeof(struct requestMsg)

static void             /* SIGCHLD handler */
grimReaper(int sig)
{
    int savedErrno;

    savedErrno = errno;                 /* waitpid() might change 'errno' */
    while (waitpid(-1, NULL, WNOHANG) > 0)
        continue;
    errno = savedErrno;
}

static void				/* SIGINT and SIGTERM handler */
sigIntTermHandler(int sig)
{
	if (mq_unlink(SERVER_MQ) == -1)
		_exit(EXIT_FAILURE);
	_exit(EXIT_SUCCESS);
}

static void             /* Executed in child process: serve a single client */
serveRequest(const struct requestMsg *req)
{
	/* Open client's message queue and requested file */
	mqd_t clientMqd = mq_open(req->clientMq, O_WRONLY);
	if (clientMqd == (mqd_t) -1)
		exit(EXIT_FAILURE);

	struct responseMsg resp;
    int fd = open(req->pathname, O_RDONLY);
    if (fd == -1) {                     /* Open failed on requested file: send error text */
        resp.mtype = RESP_MT_FAILURE;
        snprintf(resp.data, sizeof(resp.data), "%s", "Couldn't open");
		mq_send(clientMqd, (char *) &resp, RESP_SIZE(strlen(resp.data)), 0);
        exit(EXIT_FAILURE);             /* and terminate */
    }

    /* Transmit file contents in messages with type RESP_MT_DATA. We don't
       diagnose read() and mq_send() errors since we can't notify client. */

    resp.mtype = RESP_MT_DATA;
	ssize_t numRead;
    while ((numRead = read(fd, resp.data, sizeof(resp.data))) > 0)
        if (mq_send(clientMqd, (char *) &resp, RESP_SIZE(numRead), 0) == -1)
            break;

    /* Send a message of type RESP_MT_END to signify end-of-file */
    resp.mtype = RESP_MT_END;
    mq_send(clientMqd, (char *) &resp, RESP_SIZE(0), 0);
}

int
main(int argc, char *argv[])
{
	/* Block SIGINT and SIGTERM */
	sigset_t blockMask, prevMask;
	sigemptyset(&blockMask);
	sigaddset(&blockMask, SIGINT);
	sigaddset(&blockMask, SIGTERM);
	if (sigprocmask(SIG_BLOCK, &blockMask, &prevMask) == -1)
		errExit("sigprocmask() - Failed to add SIGINT and SIGTERM to process signal mask");
	
	/* Install SIGINT and SIGTERM handler to remove server queue */
	struct sigaction sigIntTermSa;
	sigemptyset(&sigIntTermSa.sa_mask);
	sigIntTermSa.sa_flags = 0;
	sigIntTermSa.sa_handler = sigIntTermHandler;
	if (sigaction(SIGINT, &sigIntTermSa, NULL) == -1)
		errExit("sigaction() - Failed to install SIGINT handler");
	if (sigaction(SIGTERM, &sigIntTermSa, NULL) == -1)
		errExit("sigaction() - Failed to install SIGTERM handler");

    /* Create server message queue */

	struct mq_attr mqAttr = {
		.mq_maxmsg  = SERVER_MQ_MAXMSG,
		.mq_msgsize = SERVER_MQ_MSGSZ,
	};
	mqd_t serverMqd = mq_open(SERVER_MQ, O_RDONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR | S_IWGRP, &mqAttr);
	if (serverMqd == (mqd_t) -1)
        errExit("mq_open() - Failed to create POSIX message queue");
	
	/* Unblock SIGINT and SIGTERM */
	if (sigprocmask(SIG_SETMASK, &prevMask, NULL) == -1)
		errExit("sigprocmask() - Failed to restore process signal mask");

    /* Establish SIGCHLD handler to reap terminated children */

	struct sigaction sigchldSa;
    sigemptyset(&sigchldSa.sa_mask);
    sigchldSa.sa_flags = SA_RESTART;
    sigchldSa.sa_handler = grimReaper;
    if (sigaction(SIGCHLD, &sigchldSa, NULL) == -1)
        errExit("sigaction");

    /* Read requests, handle each in a separate child process */

	char reqMsgBuf[SERVER_MQ_MSGSZ];
    for (;;) {
		if (mq_receive(serverMqd, reqMsgBuf, SERVER_MQ_MSGSZ, NULL) == -1) {
			if (errno == EINTR)				/* Restart */
				continue;
			errMsg("mq_receive() failed");	/* Some other error; terminate loop */
			break;
		}

        pid_t pid = fork();                   /* Create child process */
        if (pid == -1) {
            errMsg("fork");
            break;
        }

        if (pid == 0) {                 /* Child handles request */
			struct requestMsg req;
			memcpy((char *) &req, reqMsgBuf, sizeof(struct requestMsg));
            serveRequest(&req);
            _exit(EXIT_SUCCESS);
        }

        /* Parent loops to receive next client request */
    }

    /* If mq_receive() or fork() fails, remove server MQ and exit */
	if (mq_unlink(SERVER_MQ) == -1)
		errExit("mq_unlink() - Failed to remove POSIX message queue %s", SERVER_MQ);
    exit(EXIT_SUCCESS);
}

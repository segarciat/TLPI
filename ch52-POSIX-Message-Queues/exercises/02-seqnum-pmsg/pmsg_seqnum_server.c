/* Modified by Sergio E. Garcia Tapia for Exercise 52-2 */

/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2023.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Exercise 52-2 (based on Listing 44-7) */

/* pmsg_seqnum_server.c

   An example of a server using a POSIX message queue to handle client requests.
   The "service" provided is the allocation of unique sequential
   numbers. Each client submits a request consisting of its PID, and
   the length of the sequence it is to be allocated by the server.
   The PID is used by both the server and the client to construct
   the name of the POSIX message queue used by the client for receiving responses.

   The server reads each client request, and uses the client's POSIX message queue
   to send back the starting value of the sequence allocated to that
   client. The server then increments its counter of used numbers
   by the length specified in the client request.

   See pmsg_seqnum.h for the format of request and response messages.

   The client is in pmsg_seqnum_client.c.
*/
#include "pmsg_seqnum.h"
#include <signal.h>

#define SERVER_MQ_MAXMSG   10
#define SERVER_MQ_MSGSZ   128

/* Handler for SIGINT and SIGTERM. Removes the message queue and exits */
static void
signalHandler(int sig)
{
	if (mq_unlink(SERVER_MQ) == -1)
		_exit(EXIT_FAILURE);

	_exit(EXIT_SUCCESS);
}

int
main(int argc, char *argv[])
{
	/* Temporarily block SIGINT and SIGTERM */
	sigset_t blockMask, prevMask;
	sigemptyset(&blockMask);
	sigaddset(&blockMask, SIGINT);
	sigaddset(&blockMask, SIGTERM);
	if (sigprocmask(SIG_BLOCK, &blockMask, &prevMask) == -1)
		errExit("sigprocmask() - Failed to add SIGINT and SITERM to process signal mask");


	/* Create signal handler to delete queue upon receipt of SIGINT and SIGTERM before exiting */
	struct sigaction sa;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = signalHandler;
	if (sigaction(SIGINT, &sa, NULL) == -1)
		errExit("sigaction() - Failed to install SIGINT handler");
	if (sigaction(SIGTERM, &sa, NULL) == -1)
		errExit("sigaction() - Failed to install SIGTERM handler");
    
    
	/* Create well-known message queue, and open it for reading */
    umask(0);                           /* So we get the permissions we want */
	struct mq_attr srvMqAttr;
	srvMqAttr.mq_maxmsg  = SERVER_MQ_MAXMSG;
	srvMqAttr.mq_msgsize = SERVER_MQ_MSGSZ;
	mqd_t mqd = mq_open(SERVER_MQ, O_RDONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR | S_IWGRP, &srvMqAttr);
	if (mqd == -1)
		errExit("mq_open() - Failed to create server message queue");
	/* Unblock SIGINT and SIGTERM */
	if (sigprocmask(SIG_SETMASK, &prevMask, NULL) == -1)
		errExit("sigprocmask() - Failed to restore previous signal mask");

	char reqMsgBuf[SERVER_MQ_MSGSZ];
    struct request req;
    struct response resp;
	int seqNum = 0;                     /* This is our "service" */
    for (;;) {                          /* Read requests and send responses */
		ssize_t bytesRead = mq_receive(mqd, reqMsgBuf, SERVER_MQ_MSGSZ, NULL);
		if (bytesRead == -1) {	/* Ignore failure */
			fprintf(stderr, "mq_receive() - Error reading request; discarding: %s\n", strerror(errno));
			continue;
		}
		if (bytesRead == 0)		/* Empty message */
			continue;
		memcpy(&req, reqMsgBuf, sizeof(struct request));

        /* Open client POSIX message queue (previously created by client) */

    	char clientMq[CLIENT_MQ_NAME_LEN];
        snprintf(clientMq, CLIENT_MQ_NAME_LEN, CLIENT_MQ_TEMPLATE,
                (long) req.pid);
        mqd_t clientFd = mq_open(clientMq, O_WRONLY);
        if (clientFd == -1) {           /* Open failed, give up on client */
            errMsg("open %s", clientMq);
            continue;
        }

        /* Send response and close POSIX message queue */

        resp.seqNum = seqNum;
		if (mq_send(clientFd, (char *) &resp, sizeof(struct response), 0) == -1)
            fprintf(stderr, "Error writing to message queue %s\n", clientMq);

        if (mq_close(clientFd) == -1)
            errMsg("mq_close");

        seqNum += req.seqLen;           /* Update our sequence number */
    }
}

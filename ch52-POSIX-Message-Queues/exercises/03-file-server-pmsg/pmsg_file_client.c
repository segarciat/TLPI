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

/* Exercsie 52-3 (based on Listing 46-9) */

/* svmsg_file_client.c

   Send a message to the server pmsg_file_server.c requesting the
   contents of the file named on the command line, and then receive the
   file contents via a series of messages sent back by the server. Display
   the total number of bytes and messages received. The server and client
   communicate using POSIX message queues.
*/
#include "pmsg_file.h"

#define CLIENT_MQ_MSGSZ  (sizeof(struct responseMsg))
#define CLIENT_MQ_MAXMSG 10

static char clientMq[NAME_MAX + 1];

static void
removeQueue(void)
{
	if (mq_unlink(clientMq) == -1) {
		errMsg("mq_unlink() - failed to delete client queue");
		_exit(EXIT_FAILURE);
	}
}

int
main(int argc, char *argv[])
{
	/* Expect required pathname argument */
	if (argc != 2 || strcmp(argv[1], "--help") == 0)
		usageErr("%s pathname\n", argv[0]);

	/* Ensure pathname provided was not too long */
	struct requestMsg req;
    if (strlen(argv[1]) > sizeof(req.pathname) - 1)
        cmdLineErr("pathname too long (max: %ld bytes)\n",
                (long) sizeof(req.pathname) - 1);

    /* Create client queue for response */
	snprintf(clientMq, sizeof(clientMq), "/pmsg_file_client.%ld", (long) getpid());
	struct mq_attr mqAttr = {
		.mq_maxmsg  = CLIENT_MQ_MAXMSG,
		.mq_msgsize = CLIENT_MQ_MSGSZ,
	};
	mqd_t clientMqd = mq_open(clientMq, O_RDONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR | S_IWGRP, &mqAttr);
	if (clientMqd == (mqd_t) -1)
        errExit("mq_open() - client message queue");

	/* Set client queue to remove on exit */
	if (atexit(removeQueue) != 0)
        errExit("atexit");

	mqd_t serverMqd = mq_open(SERVER_MQ, O_WRONLY);
	if (serverMqd == (mqd_t) -1)
		errExit("mq_open() - server message queue");
    
	/* Send server a message asking for file named in argv[1] */
	memcpy(req.clientMq, clientMq, sizeof(req.clientMq));
    strncpy(req.pathname, argv[1], sizeof(req.pathname) - 1);
    req.pathname[sizeof(req.pathname) - 1] = '\0';
                                        /* Ensure string is terminated */
	if (mq_send(serverMqd, (char *) &req, sizeof(struct requestMsg), 0) == -1)
        errExit("mq_send");

    /* Get first response, which may be failure notification */
	char respMsgBuf[CLIENT_MQ_MSGSZ];
    ssize_t msgLen = mq_receive(clientMqd, respMsgBuf, CLIENT_MQ_MSGSZ, NULL);
	if (msgLen == -1)
		errExit("mq_receive() - Failed to receive initial message from server");
	struct responseMsg resp;
	memcpy(&resp, respMsgBuf, sizeof(struct responseMsg));

    if (resp.mtype == RESP_MT_FAILURE) {
        printf("%s\n", resp.data);      /* Display msg from server */
        exit(EXIT_FAILURE);
    }

    /* File was opened successfully by server; process messages
       (including the one already received) containing file data */

	size_t numMsgs;
    ssize_t totBytes = msgLen;                  /* Count first message */
    for (numMsgs = 1; resp.mtype == RESP_MT_DATA; numMsgs++) {
        msgLen = mq_receive(clientMqd, respMsgBuf, CLIENT_MQ_MSGSZ, NULL);
        if (msgLen == -1)
            errExit("mq_receive()");
		memcpy(&resp, respMsgBuf, sizeof(struct responseMsg));

        totBytes += msgLen;
    }

    printf("Received %ld bytes (%ld messages)\n", (long) totBytes, (long) numMsgs);

    exit(EXIT_SUCCESS);
}

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

/* Exercise 52-2 (based on Listing 44-8) */

/* pmsg_seqnum_client.c

   A simple client that uses a well-known POSIX message queue to request a (trivial)
   "sequence number service". This client creates its own message queue (using a
   convention agreed upon by client and server) which is used to receive a reply
   from the server. The client then sends a request to the server consisting of
   its PID and the length of the sequence it wishes to be allocated. The client
   then reads the server's response and displays it on stdout.

   See pmsg_seqnum.h for the format of request and response messages.

   The server is in pmsg_seqnum_server.c.
*/
#include "pmsg_seqnum.h"

#define CLIENT_MQ_MSGSZ  128
#define CLIENT_MQ_MAXMSG  10

static char clientMq[CLIENT_MQ_NAME_LEN];

static void             /* Invoked on exit to delete client POSIX message queue */
removeMq(void)
{
    mq_unlink(clientMq);
}

int
main(int argc, char *argv[])
{
	/* Validate command-line arguments */
    if (argc > 1 && strcmp(argv[1], "--help") == 0)
        usageErr("%s [seq-len]\n", argv[0]);

    /* Create our POSIX message queue (before sending request, to avoid a race) */

    umask(0);                   /* So we get the permissions we want */

	/* Build message queue name */
    snprintf(clientMq, CLIENT_MQ_NAME_LEN, CLIENT_MQ_TEMPLATE,
            (long) getpid());

	/* Set message queue attributes */
	struct mq_attr mqAttr = {
		.mq_maxmsg  = CLIENT_MQ_MAXMSG,
		.mq_msgsize = CLIENT_MQ_MSGSZ,
	};

	/* Create message queue and set to delete on exit */
	mqd_t clientMqd = mq_open(clientMq, O_RDONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR | S_IWGRP, &mqAttr);
	if (clientMqd == -1)
        errExit("mq_open %s", clientMq);

    if (atexit(removeMq) != 0)
        errExit("atexit");

    /* Construct request message, open server message queue, and send message */

    struct request req = {
		.pid = getpid(),
    	.seqLen = (argc > 1) ? getInt(argv[1], GN_GT_0, "seq-len") : 1,
	};

	mqd_t serverMqd = mq_open(SERVER_MQ, O_WRONLY);
	if (serverMqd == -1)
        errExit("mq_open %s", SERVER_MQ);

	if (mq_send(serverMqd, (char *) &req, sizeof(struct request), 0) == -1)
        fatal("Can't write to server");

    /* Read queue */
    struct response resp;
	char respMsgBuf[CLIENT_MQ_MSGSZ];
	if (mq_receive(clientMqd, respMsgBuf, CLIENT_MQ_MSGSZ, 0) == -1)
        fatal("Can't read response from server");
	memcpy(&resp, respMsgBuf, sizeof(struct response));

	/* Display response */ 
    printf("%d\n", resp.seqNum);
    exit(EXIT_SUCCESS);
}

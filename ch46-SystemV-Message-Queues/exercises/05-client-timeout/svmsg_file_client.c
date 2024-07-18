/* Modified by Sergio E. Garcia Tapia for Exercise 46.5 */

/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2023.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 46-9 */

/* svmsg_file_client.c

   Send a message to the server svmsg_file_server.c requesting the
   contents of the file named on the command line, and then receive the
   file contents via a series of messages sent back by the server. Display
   the total number of bytes and messages received. The server and client
   communicate using System V message queues.
*/
#include "svmsg_file.h"
#include <unistd.h> /* alarm() */
#include <signal.h>
#include <errno.h>

#define MQ_TIMEOUT 1

static int clientId;

static void						/* Remove client queue on exit */
removeQueue(void)
{
    if (msgctl(clientId, IPC_RMID, NULL) == -1)
        errExit("msgctl");
}

static void						/* SIGALRM handler */
timeoutHandler(int sig)
{
}

static int						/* Read server's message queue ID from well-known file */
getServerMessageQueueId()
{
	int serverId;
	FILE *servermqIdFile;
	char line[BUFSIZ];
	char *endp;

	servermqIdFile = fopen(SERVER_MQID_FILE, "r");
	if (servermqIdFile == NULL)
		errExit("fopen() - failed to open server message queue id file");
	if (fgets(line, BUFSIZ, servermqIdFile) == NULL)
		errExit("fgets() - failed to read server message queue id file");
	errno = 0;
	serverId = strtol(line, &endp, /* Any base */ 0);
	if (errno != 0 || *endp != '\0')
		fatal("strtol() - error or invalid id while parsing server message queue id file");
	return serverId;
}

int
main(int argc, char *argv[])
{
    struct requestMsg req;
    struct responseMsg resp;
    int serverId, numMsgs;
    ssize_t msgLen, totBytes;
	struct msqid_ds ds;
	struct sigaction sa;
	int savedErrno;

    if (argc < 2 || argc > 3 || strcmp(argv[1], "--help") == 0)
        usageErr("%s pathname [max-bytes]\n", argv[0]);

    if (strlen(argv[1]) > sizeof(req.pathname) - 1)
        cmdLineErr("pathname too long (max: %ld bytes)\n",
                (long) sizeof(req.pathname) - 1);

    /* Get server's queue identifier; create queue for response */
	serverId = getServerMessageQueueId();
    clientId = msgget(IPC_PRIVATE, S_IRUSR | S_IWUSR | S_IWGRP);
	if (clientId == -1)
        errExit("msgget - client message queue");

	if (atexit(removeQueue) != 0)
        errExit("atexit");
	
	if (argc == 3) {
		if (msgctl(clientId, IPC_STAT, &ds) == -1)
			errExit("msgctl");
		
		ds.msg_qbytes = getInt(argv[2], 0, "max-bytes");
		if (msgctl(clientId, IPC_SET, &ds) == -1)
			errExit("msgctl");
	}

    /* Send message asking for file named in argv[1] */

    req.mtype = 1;                      /* Any type will do */
    req.clientId = clientId;
    strncpy(req.pathname, argv[1], sizeof(req.pathname) - 1);
    req.pathname[sizeof(req.pathname) - 1] = '\0';
                                        /* Ensure string is terminated */

	/* Install SIGALRM handler for timeouts */
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = timeoutHandler;
	if (sigaction(SIGALRM, &sa, NULL) == -1)
		errExit("sigaction - failed to install SIGALRM handler");

	alarm(MQ_TIMEOUT);
	if (msgsnd(serverId, &req, REQ_MSG_SIZE, 0) == -1)
        errExit("msgsnd");
	savedErrno = errno;
	alarm(0);		/* Disable alarm */
	errno = savedErrno;

    /* Get first response, which may be failure notification */

	alarm(MQ_TIMEOUT);
    msgLen = msgrcv(clientId, &resp, RESP_MSG_SIZE, 0, 0);
	savedErrno = errno;
	alarm(0);		/* Disable alarm */
	errno = savedErrno;
    if (msgLen == -1)
        errExit("msgrcv");

    if (resp.mtype == RESP_MT_FAILURE) {
        printf("%s\n", resp.data);      /* Display msg from server */
        exit(EXIT_FAILURE);
    }

    /* File was opened successfully by server; process messages
       (including the one already received) containing file data */

    totBytes = msgLen;                  /* Count first message */
    for (numMsgs = 1; resp.mtype == RESP_MT_DATA; numMsgs++) {
		alarm(MQ_TIMEOUT);
        msgLen = msgrcv(clientId, &resp, RESP_MSG_SIZE, 0, 0);
		savedErrno = errno;
		alarm(0);
		errno = savedErrno;
        if (msgLen == -1)
            errExit("msgrcv");

        totBytes += msgLen;
    }

    printf("Received %ld bytes (%d messages)\n", (long) totBytes, numMsgs);

    exit(EXIT_SUCCESS);
}

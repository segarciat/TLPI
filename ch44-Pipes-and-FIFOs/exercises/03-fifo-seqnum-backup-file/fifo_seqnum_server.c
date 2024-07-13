/* Modified by Sergio E. Garcia Tapia for Exercise 44.3 */
/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2023.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 44-7 */

/* fifo_seqnum_server.c

   An example of a server using a FIFO to handle client requests.
   The "service" provided is the allocation of unique sequential
   numbers. Each client submits a request consisting of its PID, and
   the length of the sequence it is to be allocated by the server.
   The PID is used by both the server and the client to construct
   the name of the FIFO used by the client for receiving responses.

   The server reads each client request, and uses the client's FIFO
   to send back the starting value of the sequence allocated to that
   client. The server then increments its counter of used numbers
   by the length specified in the client request.

   See fifo_seqnum.h for the format of request and response messages.

   The client is in fifo_seqnum_client.c.
*/
#include <signal.h>
#include "fifo_seqnum.h"

#define SEQNUM_BACKUP_FILE "./seqnum_srv.backup"
#define SEQNUM_TEXT_LEN 100

int
main(int argc, char *argv[])
{
    int serverFd, dummyFd, clientFd, seqnumBackupFd;
    char clientFifo[CLIENT_FIFO_NAME_LEN];
	char seqnumBackupBuf[SEQNUM_TEXT_LEN];
	ssize_t bytesRead;
    struct request req;
    struct response resp;
    int seqNum;                     /* This is our "service" */

    umask(0);                           /* So we get the permissions we want */

	/* Create sequence number backup file */
	seqnumBackupFd = open(SEQNUM_BACKUP_FILE, O_RDWR | O_CREAT | O_SYNC, S_IRUSR | S_IWUSR);
	if (seqnumBackupFd == -1)
		errExit("open seqnum backup");
	
	bytesRead = read(seqnumBackupFd, seqnumBackupBuf, SEQNUM_TEXT_LEN - 1);
	if (bytesRead == -1)
		errExit("read seqnum backup");
	if (bytesRead == 0) { /* No previous sequence number in backup file */
		printf("Starting from 0\n");
		seqNum = 0;
	}
	else {
		/* Read starting seqnum */
		seqnumBackupBuf[bytesRead] = '\0';
		seqNum = getInt(seqnumBackupBuf, GN_NONNEG, "seqnum-backup");
		printf("read starting seqnum: %d\n", seqNum);
	}

    /* Create well-known FIFO, and open it for reading */
    if (mkfifo(SERVER_FIFO, S_IRUSR | S_IWUSR | S_IWGRP) == -1
            && errno != EEXIST)
        errExit("mkfifo %s", SERVER_FIFO);
    serverFd = open(SERVER_FIFO, O_RDONLY);
    if (serverFd == -1)
        errExit("open %s", SERVER_FIFO);

    /* Open an extra write descriptor, so that we never see EOF */

    dummyFd = open(SERVER_FIFO, O_WRONLY);
    if (dummyFd == -1)
        errExit("open %s", SERVER_FIFO);

    /* Let's find out about broken client pipe via failed write() */

    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)    errExit("signal");

    for (;;) {                          /* Read requests and send responses */
        if (read(serverFd, &req, sizeof(struct request))
                != sizeof(struct request)) {
            fprintf(stderr, "Error reading request; discarding\n");
            continue;                   /* Either partial read or error */
        }

        /* Open client FIFO (previously created by client) */

        snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE,
                (long) req.pid);
        clientFd = open(clientFifo, O_WRONLY);
        if (clientFd == -1) {           /* Open failed, give up on client */
            errMsg("open %s", clientFifo);
            continue;
        }

        /* Send response and close FIFO */

        resp.seqNum = seqNum;
        if (write(clientFd, &resp, sizeof(struct response))
                != sizeof(struct response))
            fprintf(stderr, "Error writing to FIFO %s\n", clientFifo);
        if (close(clientFd) == -1)
            errMsg("close");

        seqNum += req.seqLen;           /* Update our sequence number */
		printf("next seqNum: %d\n", seqNum);
		/* Update starting sequence number in backup file */
		memset(seqnumBackupBuf, 0, SEQNUM_TEXT_LEN);
		int seqnumLen = snprintf(seqnumBackupBuf, SEQNUM_TEXT_LEN, "%d", seqNum);
		if (lseek(seqnumBackupFd, 0, SEEK_SET) == -1)
			errExit("lseek failed: failed to backup seqNum %d\n", seqNum);
		if (write(seqnumBackupFd, seqnumBackupBuf, seqnumLen) != seqnumLen)
			errExit("write failed: unable to backup seqNum %d\n", seqNum);
    }
}

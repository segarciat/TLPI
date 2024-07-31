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

/* Exercise 52-3 (based on Listing 46-7) */

/* pmsg_file.h

   Header file for pmsg_file_server.c and pmsg_file_client.c.
*/
#include <sys/types.h>					/* ssize_t, etc */
#include <sys/stat.h>					/* umask() */
#include <limits.h>						/* PATH_MAX, NAME_MAX */
#include <fcntl.h>						/* O_* flags */
#include <signal.h>						/* sigaction(), etc */
#include <mqueue.h>						/* mq_open(), mq_unlink() mq_send(), mq_receive(), etc */
#include <stddef.h>						/* offsetof() */
#include "tlpi_hdr.h"

#define SERVER_MQ "/pmsg_file_srv"		/* Key for server's message queue */

struct requestMsg {                     /* Requests (client to server) */
	char clientMq[NAME_MAX + 1];		/* ID of client's message queue */
    char pathname[PATH_MAX];            /* File to be returned */
};

#define RESP_MSG_SIZE 4096

struct responseMsg {                    /* Responses (server to client) */
    long mtype;                         /* One of RESP_MT_* values below */
    char data[RESP_MSG_SIZE];           /* File content / response message */
};

#define RESP_HDR_SIZE (offsetof(struct responseMsg, data) - \
						offsetof(struct responseMsg, mtype))
#define RESP_SIZE(data_size) (data_size + RESP_HDR_SIZE)

/* Types for response messages sent from server to client */

#define RESP_MT_FAILURE 1               /* File couldn't be opened */
#define RESP_MT_DATA    2               /* Message contains file data */
#define RESP_MT_END     3               /* File data complete */

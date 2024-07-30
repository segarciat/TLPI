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

/* Exercise 52-2 (based on Listing 44-6) */

/* pmsg_seqnum.h (based on fifo_seqnum.h)

   Header file used by pmsg_seqnum_server.c and pmsg_seqnum_client.c
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mqueue.h>
#include "tlpi_hdr.h"

#define SERVER_MQ "/seqnum_mq_srv"
                                /* Well-known name for server's POSIX message queue */
#define CLIENT_MQ_TEMPLATE "/seqnum_mq_cl.%ld"
                                /* Template for building client POSIX message queue name */
#define CLIENT_MQ_NAME_LEN (sizeof(CLIENT_MQ_TEMPLATE) + 20)
                                /* Space required for client FIFO pathname
                                  (+20 as a generous allowance for the PID) */

struct request {                /* Request (client --> server) */
    pid_t pid;                  /* PID of client */
    int seqLen;                 /* Length of desired sequence */
};

struct response {               /* Response (server --> client) */
    int seqNum;                 /* Start of sequence */
};

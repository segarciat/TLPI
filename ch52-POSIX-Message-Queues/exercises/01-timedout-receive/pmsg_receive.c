/* Modified by Sergio E. Garcia Tapai for Exercise 52-1 */

/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2023.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Exercise 52-1 (based on Listing 52-5) */

/* pmsg_receive.c

   Usage as shown in usageError().

   Receive a message from a POSIX message queue, and write it on
   standard output.

   See also pmsg_send.c.

   Linux supports POSIX message queues since kernel 2.6.6.
*/
#include <mqueue.h>
#include <fcntl.h>             	/* For definition of O_NONBLOCK */
#include "tlpi_hdr.h"
#define _POSIX_C_SOURCE 200809L	/* Expose POSIX clocks API (must be at least 199309L) */
#include <time.h>

static void
usageError(const char *progName)
{
    fprintf(stderr, "Usage: %s [-n] mq-name [secs-pmsgr-timeout [nsecs-pmsgr-timemout]]\n", progName);
    fprintf(stderr, "    -n           Use O_NONBLOCK flag\n");
    exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
    int flags, opt;
    mqd_t mqd;
    unsigned int prio;
    void *buffer;
    struct mq_attr attr;
    ssize_t numRead;

    flags = O_RDONLY;
    while ((opt = getopt(argc, argv, "n")) != -1) {
        switch (opt) {
        case 'n':   flags |= O_NONBLOCK;        break;
        default:    usageError(argv[0]);
        }
    }

    if (optind >= argc)
        usageError(argv[0]);

    mqd = mq_open(argv[optind], flags);
    if (mqd == (mqd_t) -1)
        errExit("mq_open");

    /* We need to know the 'mq_msgsize' attribute of the queue in
       order to determine the size of the buffer for mq_receive() */

    if (mq_getattr(mqd, &attr) == -1)
        errExit("mq_getattr");

    buffer = malloc(attr.mq_msgsize);
	if (buffer == NULL)
        errExit("malloc");
	
	if (argc > optind + 1) { /* User supplied by a timeout */
		struct timespec timeoutTp;
		if (clock_gettime(CLOCK_REALTIME, &timeoutTp) == -1)
			errExit("clock_gettime() - Failed to get current value for CLOCK_REALTIME");
		timeoutTp.tv_sec  += getLong(argv[optind + 1], GN_NONNEG, "secs-pmsgr-timeout");
		timeoutTp.tv_nsec += (argc <= optind + 2) ? 0 : getLong(argv[optind + 1], GN_NONNEG, "secs-pmsgr-timeout");
    	numRead = mq_timedreceive(mqd, buffer, attr.mq_msgsize, &prio, &timeoutTp);
	} else					/* No timeout supplied */
		numRead = mq_receive(mqd, buffer, attr.mq_msgsize, &prio);
    if (numRead == -1)
        errExit("mq_receive");

    printf("Read %ld bytes; priority = %u\n", (long) numRead, prio);
    if (write(STDOUT_FILENO, buffer, numRead) == -1)
        errExit("write");
    write(STDOUT_FILENO, "\n", 1);

    exit(EXIT_SUCCESS);
}

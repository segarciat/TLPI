/* Modified by Sergio E. Garcia Tapia for Exercise 53-1 */

/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2023.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Exercise 53-1 (based on Listing 48-2 and Listing 48-3) */

/*  psem_xfr.c

   Read buffers of data from standard input into a global buffer dynamically
   allocated on the heap. The global buffer is shared by the main thread and
   a peer thread, and access to it and other global variables is controlled by
   an unnamed POSIX. The data is read into the buffer from standard input is
   written to standard outpy the peer thread.
*/

#include <sys/types.h>
#include <stdio.h>
#include <semaphore.h>			/* sem_init() */
#include <stdlib.h>				/* calloc() */
#include <pthread.h>			/* pthread_t */
#include <unistd.h>				/* read(), STDIN_FILENO */
#include "tlpi_hdr.h"

#define BUF_SIZE 1024

static sem_t readerSem;
static sem_t writerSem;
static char *buf;
static ssize_t bytesRead = 0;

static void*
threadFunc(void *arg)
{
	int bytes = 0;
	int xfrs = 0;
	for (;;) {
		/* Acqurie reader semaphore */
		if (sem_wait(&readerSem) == -1)
			errExit("sem_wait() - Peer thread failed to acquire semaphore");
		/* Break if no more bytes available */
		if (bytesRead == 0)
			break;
		bytes += bytesRead;
		xfrs += 1;
		/* Write the data to stdout and post to writer semaphore */
		if (write(STDOUT_FILENO, buf, bytesRead) != bytesRead)
			fatal("partial/failed write");
		if (sem_post(&writerSem) == -1)
			errExit("sem_post() - Peer thread failed to post to writer semaphore");
	}
	/* Let writer finish up */
	if (sem_post(&writerSem) == -1)
		errExit("sem_post() - Peer thread failed to post to writer semaphore");
	fprintf(stderr, "Received %d bytes (%d xfers)\n", bytes, xfrs);
	return NULL;
}

int
main(int argc, char *argv[])
{	
	/* Initialize buffer */
	buf = calloc(BUF_SIZE, sizeof(char));
	if (buf == NULL)
		errExit("calloc() - Failed to initialize buffer");
	
	/* Initialize unnamed semaphore */
	if (sem_init(&readerSem, /* thread-shared */ 0, /* Unavailable */ 0) == -1)
		errExit("sem_init() - Failed to initialize unnamed semaphore for reader");
	if (sem_init(&writerSem, /* thread-shared */ 0, /* Available */ 1) == -1)
		errExit("sem_init() - Failed to initialize unnamed semaphore for writer");
	
	/* Spawn peer thread */
	pthread_t thr;
	int s = pthread_create(&thr, NULL, threadFunc, NULL);
	if (s != 0)
		errExitEN(s, "pthread_create() - Failed to create thread");
	
	int xfrs = 0;
	int bytes = 0;

	/* Copy stdin to buffer */
	for (;;) {
		if (sem_wait(&writerSem) == -1)
			errExit("sem_wait() - Failed to acquire semaphore in writer");
		bytesRead = read(STDIN_FILENO, buf, BUF_SIZE);
		if (bytesRead == -1)
			errExit("read()");
		if (sem_post(&readerSem) == -1)
			errExit("sem_post() - Failed to post to reader semaphore from writer");
		if (bytesRead == 0)
			break;
		bytes += bytesRead;
		xfrs += 1;
	}

	/* Clean up */
	if (sem_wait(&writerSem) == -1)
		errExit("sem_wait() - Failed to acquire unnamed semaphore in writer");
	if (sem_destroy(&writerSem) == -1)
		errExit("sem_destroy() - Failed to destroy unnamed semaphore for writer");
	if (sem_destroy(&readerSem) == -1)
		errExit("sem_destroy() - Failed to destroy unnamed semaphore for reader");

	/* Reap peer thread */
	s = pthread_join(thr, NULL);
	if (s == -1)
		errExit("pthread_join() - Failed to reap peer thread");
	free(buf);
    fprintf(stderr, "Sent %d bytes (%d xfrs)\n", bytes, xfrs);
}

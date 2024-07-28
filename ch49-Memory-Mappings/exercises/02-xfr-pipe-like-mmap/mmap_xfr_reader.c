/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2023.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Exercise 49-2 */

/* mmap_xfr_reader.c

   Read data from a shared file mapping using a binary semaphore lock-step
   protocol; see mmap_xfr_writer.c
*/
#include "mmap_xfr.h"

int
main(int argc, char *argv[])
{
    /* Get IDs for semaphore set */

    int semid = semget(SEM_KEY, 0, 0);
	if (semid == -1)
        errExit("semget");

	/* Created read-only shared file mapping from file created by writer */
	int fd = open(MMAP_XFR_SHARED_FILE, O_RDONLY);
	if (fd == -1)
		errExit("open() - failed to open shared file");

	struct mmap_region *mmapAddr = mmap(NULL, sizeof(struct mmap_region), PROT_READ, MAP_SHARED, fd, 0);
	if (mmapAddr == MAP_FAILED)
		errExit("mmap() - failed to create shared file mapping");
	if (close(fd == -1))
		errExit("close() - failed to close shared file");

    /* Transfer blocks of data from shared memory to stdout */

    int xfrs, bytes;
    for (xfrs = 0, bytes = 0; ; xfrs++) {
        if (reserveSem(semid, READ_SEM) == -1)          /* Wait for our turn */
            errExit("reserveSem");

        if (mmapAddr->cnt == 0)                     /* Writer encountered EOF */
            break;
        bytes += mmapAddr->cnt;

        if (write(STDOUT_FILENO, mmapAddr->buf, mmapAddr->cnt) != mmapAddr->cnt)
            fatal("partial/failed write");

        if (releaseSem(semid, WRITE_SEM) == -1)         /* Give writer a turn */
            errExit("releaseSem");
    }

	if (munmap(mmapAddr, sizeof(struct mmap_region)) == -1)
		errExit("munmap() - failed to remove shared file mapping");
    /* Give writer one more turn, so it can clean up */

    if (releaseSem(semid, WRITE_SEM) == -1)
        errExit("releaseSem");

    fprintf(stderr, "Received %d bytes (%d xfrs)\n", bytes, xfrs);
    exit(EXIT_SUCCESS);
}

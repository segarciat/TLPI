/* Modified by Sergio E. Garcia Tapia for Exercise 54-1 */

/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2023.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Exercise 54-1 (Listing 48-3) */

/* pshm_xfr_reader.c

   Read data from a POSIX shared memory using a binary semaphore lock-step
   protocol; see pshm_xfr_writer.c
*/
#include "pshm_xfr.h"

int
main(int argc, char *argv[])
{
    /* Open POSIX semaphores */
    sem_t* writeSem = sem_open(XFR_WRITE_SEM, O_RDWR);
    if (writeSem == SEM_FAILED)
        errExit("sem_open() - Failed to open write semaphore");
    sem_t* readSem = sem_open(XFR_READ_SEM, O_RDWR);
    if (readSem == SEM_FAILED)
        errExit("sem_open() - Failed to open read semaphore");

    /* Open and map POSIX shared memory object */
    int pshmfd = shm_open(XFR_PSHM, O_RDONLY, 0);
    if (pshmfd == -1)
        errExit("shm_open() - Failed to create POSIX shared memory object");
    struct pshmobj* pshmAddr = mmap(NULL, sizeof(struct pshmobj), PROT_READ, MAP_SHARED, pshmfd, 0);
    if (pshmAddr == MAP_FAILED)
        errExit("mmap() - Failed to created shared file mapping");
    if (close(pshmfd) == -1)
        errExit("close() - Failed to close POSIX shared memory object file descriptor");

    /* Transfer blocks of data from shared memory to stdout */
    int xfrs = 0;
    ssize_t bytes = 0;
    for (; ; xfrs++) {
        if (sem_wait(readSem) == -1)                    /* Wait for our turn */
            errExit("sem_wait() - Failed to wait for read semaphore");

        if (pshmAddr->cnt == 0)                     /* Writer encountered EOF */
            break;
        bytes += pshmAddr->cnt;

        if (write(STDOUT_FILENO, pshmAddr->buf, pshmAddr->cnt) != pshmAddr->cnt)
            fatal("partial/failed write");

        if (sem_post(writeSem) == -1)         /* Give writer a turn */
            errExit("sem_post() - Failed to post to write semaphore");
    }

    if (munmap(pshmAddr, sizeof(struct pshmobj)) == -1)
        errExit("munmap() - Failed to unmap shared memory region");

    /* Give writer one more turn, so it can clean up */
    if (sem_post(writeSem) == -1)
        errExit("sem_post() - Failed to post to write semaphore");

    fprintf(stderr, "Received %lld bytes (%d xfrs)\n", (long long) bytes, xfrs);
    exit(EXIT_SUCCESS);
}

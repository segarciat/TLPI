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

/* Exercise 54-1 (based on Listing 48-2) */

/*  pshm_xfr_writer.c

   Read buffers of data data from standard input into a POSIX shared memory
   object from which it is copied by pshm_xfr_reader.c

   We use a pair of binary semaphores to ensure that the writer and reader have
   exclusive, alternating access to the shared memory. (I.e., the writer writes
   a block of text, then the reader reads, then the writer writes etc). This
   ensures that each block of data is processed in turn by the writer and
   reader.

   This program needs to be started before the reader process as it creates the
   shared memory and semaphores used by both processes.

   Together, these two programs can be used to transfer a stream of data through
   shared memory as follows:

        $ pshm_xfr_writer < infile &
        $ pshm_xfr_reader > out_file
*/
#include "pshm_xfr.h"

int
main(int argc, char *argv[])
{
    /* Create two POSIX semaphores; initialize so that
       writer has first access to shared memory. */
    sem_t* writeSem = sem_open(XFR_WRITE_SEM, O_RDWR | O_CREAT, OBJ_PERMS, 1);
    if (writeSem == SEM_FAILED)
        errExit("sem_open() - Failed to create write semaphore");
    
    sem_t* readSem = sem_open(XFR_READ_SEM, O_RDWR | O_CREAT, OBJ_PERMS, 0);
    if (readSem == SEM_FAILED)
        errExit("sem_open() - Failed to create read semaphore");

    /* Create shared memory object and set its size */
    int pshmfd = shm_open(XFR_PSHM, O_CREAT | O_RDWR , OBJ_PERMS);
    if (pshmfd == -1)
        errExit("shm_open() - Failed to create shared memory object");

    if (ftruncate(pshmfd, sizeof(struct pshmobj)) == -1)
        errExit("ftruncate() - Failed to set size of shared memory object");

    struct pshmobj* pshmAddr = mmap(NULL, sizeof(struct pshmobj), PROT_READ | PROT_WRITE, MAP_SHARED, pshmfd, 0);
    if (pshmAddr == MAP_FAILED)
        errExit("mmap() - Failed to create a shared memory mapping");
    if (close(pshmfd) == -1)
        errExit("close() - Failed to close POSIX semaphore memory file descriptor");

    /* Transfer blocks of data from stdin to shared memory */
    int xfrs = 0;
    ssize_t bytes = 0;
    for (; ; xfrs++, bytes += pshmAddr->cnt) {
        if (sem_wait(writeSem) == -1)
            errExit("sem_wait() - Failed to acquire write semaphore");

        pshmAddr->cnt = read(STDIN_FILENO, pshmAddr->buf, BUF_SIZE);
        if (pshmAddr->cnt == -1)
            errExit("read() - Failed to read from stdin into shared memory object");

        if (sem_post(readSem) == -1)          /* Give reader a turn */
            errExit("sem_post() - Failed to post to read semaphore");

        /* Have we reached EOF? We test this after giving the reader
           a turn so that it can see the 0 value in pshmAddr->cnt. */

        if (pshmAddr->cnt == 0)
            break;
    }

    if (munmap(pshmAddr, sizeof(struct pshmobj)) == -1)
        errExit("munmap() - Failed to unmap shared memory region");

    /* Wait until reader has let us have one more turn. We then know
       reader has finished, and so we can delete the IPC objects. */

    if (sem_wait(writeSem) == -1)
        errExit("sem_wait() - Failed to reserve write semaphore");
    if (sem_close(writeSem) == -1)
        errExit("sem_close() - Failed to close write semaphore");
    if (sem_close(readSem) == -1)
        errExit("sem_close() - Failed to close read semaphore");

    if (sem_unlink(XFR_WRITE_SEM) == -1)
        errExit("sem_unlink() - Failed to remove write semaphore");
    if (sem_unlink(XFR_READ_SEM) == -1)
        errExit("sem_unlink() - Failed to remove read semaphore");
    if (shm_unlink(XFR_PSHM) == -1)
        errExit("shm_unlink() - Failed to remove POSIX shared memory object");

    fprintf(stderr, "Sent %lld bytes (%d xfrs)\n", (long long) bytes, xfrs);
    exit(EXIT_SUCCESS);
}

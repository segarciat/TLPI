/* Modified by Sergio E. Garcia Tapia for Exercise 49-2 */


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

/*  mmap_xfr_writer.c

   Read buffers of data data from standard input into a shared file mapping
   from which it is copied by mmap_xfr_reader.c

   We use a pair of binary semaphores to ensure that the writer and reader have
   exclusive, alternating access to the shared memory. (I.e., the writer writes
   a block of text, then the reader reads, then the writer writes etc). This
   ensures that each block of data is processed in turn by the writer and
   reader.

   This program needs to be started before the reader process as it creates the
   file used for the shared file mapping and semaphores used by both processes.

   Together, these two programs can be used to transfer a stream of data through
   shared memory as follows:

        $ mmap_xfr_writer < infile &
        $ mmap_xfr_reader > out_file
*/
#include "semun.h"              /* Definition of semun union */
#include "mmap_xfr.h"

void
removeMappedFile()
{
	if (unlink(MMAP_XFR_SHARED_FILE) == -1)
		fprintf(stderr, "[%ld] Failed to delete %s: %s\n", (long) getppid(), MMAP_XFR_SHARED_FILE, strerror(errno));
}

int
main(int argc, char *argv[])
{
    /* Create set containing two semaphores; initialize so that
       writer has first access to shared memory mapping. */

    int semid = semget(SEM_KEY, 2, IPC_CREAT | OBJ_PERMS);
	if (semid == -1)
        errExit("semget");

	if (initSemAvailable(semid, WRITE_SEM) == -1)
        errExit("initSemAvailable");
	if (initSemInUse(semid, READ_SEM) == -1)
        errExit("initSemInUse");

	/* Create file used for shared memory mapping create a shared file mapping */
	int fd = open(MMAP_XFR_SHARED_FILE, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP);
	if (fd == -1)
		errExit("open() - failed to create file used for memory mapping");
	if (atexit(removeMappedFile) == -1)
		errExit("atexit() - failed to set exit handler to delete shared mapped file");
	if (ftruncate(fd, sizeof(struct mmap_region)) == -1)
		errExit("ftruncate() - failed to set size of shared file");
    struct mmap_region *mmapAddr = mmap(NULL, sizeof(struct mmap_region),
											PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (mmapAddr == MAP_FAILED)
        errExit("mmap() - failed to create shared memory mapping");

    /* Transfer blocks of data from stdin to shared memory */
	int bytes, xfrs;
	for (xfrs = 0, bytes = 0; ; xfrs++, bytes += mmapAddr->cnt) {
        if (reserveSem(semid, WRITE_SEM) == -1)         /* Wait for our turn */
            errExit("reserveSem");

        mmapAddr->cnt = read(STDIN_FILENO, mmapAddr->buf, BUF_SIZE);
        if (mmapAddr->cnt == -1)
            errExit("read");

        if (releaseSem(semid, READ_SEM) == -1)          /* Give reader a turn */
            errExit("releaseSem");

        /* Have we reached EOF? We test this after giving the reader
           a turn so that it can see the 0 value in mmapAddr->cnt. */

        if (mmapAddr->cnt == 0)
            break;
    }

    /* Wait until reader has let us have one more turn. We then know
       reader has finished, and so we can delete the IPC objects. */

	if (reserveSem(semid, WRITE_SEM) == -1)
        errExit("reserveSem");
	if (close(fd == -1))
		errExit("close() - failed to close shared input file");

    union semun dummy;
	if (semctl(semid, 0, IPC_RMID, dummy) == -1)
        errExit("semctl");
	if (munmap(mmapAddr, sizeof(struct mmap_region)) == -1)
		errExit("munumap() - failed to remove shared memory mapping");

    fprintf(stderr, "Sent %d bytes (%d xfrs)\n", bytes, xfrs);
    exit(EXIT_SUCCESS);
}

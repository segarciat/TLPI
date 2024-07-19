/* Modified by Sergio E. Garcia Tapia for Exercise 47-2 */

/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2023.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 24-6 */
/*
   Demonstrate how semaphores can be used to synchronize the actions
   of a parent and child process.
*/
#include "curr_time.h"	/* Declaration of currTime() */
#include "tlpi_hdr.h"

#include <sys/types.h>	/* For portabiity */
#include <sys/stat.h>	/* Permission flags */
#include <sys/sem.h>	/* semget() */

#if (_SEM_SEMUN_UNDEFINED == 1)
union semun {
	int val;
	struct semid_ds *buf;
	unsigned short *array;
#if defined(__linux__)
	struct seminfo *__buf;
#endif
};
#endif

int
main(int argc, char *argv[])
{
    pid_t childPid;
	int semId;
	union semun semarg;
	struct sembuf sop;

	/* Create semaphore set with a single semaphore */
	semId = semget(IPC_PRIVATE, /* Single semaphore */ 1, S_IRUSR | S_IWUSR);
	if (semId == -1)
		errExit("semget() - Failed to create semaphore");
	printf("Created sempahore set with id: %ld\n", (long) semId);

	/* Initialize semaphore to 0, semantically meaning it is initially reserved by child */
	semarg.val = 0;
	if (semctl(semId, /* sumnum= */ 0, SETVAL, semarg) == -1)
		errExit("semctl");

    setbuf(stdout, NULL);               /* Disable buffering of stdout */

    switch (childPid = fork()) {
    case -1:
        errExit("fork");

    case 0: /* Child */

        /* Child does some required action here... */
        printf("[%s %ld] Child started - doing some work\n",
                currTime("%T"), (long) getpid());
        sleep(2);               /* Simulate time spent doing some work */

        /* And then signals parent that it's done */

        printf("[%s %ld] Child about to release semaphore to allow parent to continue\n",
                currTime("%T"), (long) getpid());
		sop.sem_num = 0;	/* Semaphore 0 */
		sop.sem_op = 1;		/* Increase semaphore value (free) */
		sop.sem_flg = 0;	/* No special flags */
		if (semop(semId, &sop, /* Number of operations */ 1) == -1)
			errExit("semopd failed");

        /* Now child can do other things... */

        _exit(EXIT_SUCCESS);

    default: /* Parent */

        /* Parent may do some work here, and then waits for child to
           complete the required action */
        printf("[%s %ld] Parent about to semop\n",
                currTime("%T"), (long) getpid());

		sop.sem_num = 0;	/* Semaphore 0 */
		sop.sem_op = -1;	/* Waiting to decrease semaphore value (reserve) */
		sop.sem_flg = 0;	/* No special flags */

		if (semop(semId, &sop, /* Number of operations */ 1) == -1)
			errExit("semop failed");

        printf("[%s %ld] Parent semop succeeded\n", currTime("%T"), (long) getpid());
        /* Parent carries on to do other things... */
		printf("[%s %ld] Parent deleting semaphore and exiting\n", currTime("%T"), (long) getpid());
		if (semctl(semId, /* Ignored */ 0, IPC_RMID, /* Ignored */ semarg) == -1)
			errExit("semctl - Failed to remove semaphore with ID %ld", (long) semId);

        exit(EXIT_SUCCESS);
    }
}

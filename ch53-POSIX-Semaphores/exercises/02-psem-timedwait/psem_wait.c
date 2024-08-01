/* Modified by Sergio E. Garcia Tapia for Exercise 53-2 */

/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2023.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Exercise 53-2 (based on Listing 53-3) */

/* psem_wait.c

   Decrease the value of a POSIX named semaphore.

   See also psem_post.c.

   On Linux, named semaphores are supported with kernel 2.6 or later, and
   a glibc that provides the NPTL threading implementation.
*/
#include <semaphore.h>
#include "tlpi_hdr.h"
#define _POSIX_C_SOURCE 200809L
#include <time.h>

int
main(int argc, char *argv[])
{
    sem_t *sem;

    if (argc < 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s sem-name [sec-psem-timeout [nsec-psem-timeout]]\n", argv[0]);

    sem = sem_open(argv[1], 0);
	if (sem == SEM_FAILED)
		errExit("sem_open");
	
	struct timespec timeoutTs;
	if (clock_gettime(CLOCK_REALTIME, &timeoutTs) == -1)
		errExit("clock_gettime() - CLOCK_REALTIME: Failed to get current time");
	if (argc > 2) {
		timeoutTs.tv_sec  += getLong(argv[2], GN_NONNEG, "sec-psem-timeout");
		timeoutTs.tv_nsec += (argc > 3) ? 0 : getLong(argv[2], GN_NONNEG, "sec-psem-timeout");
		if (sem_timedwait(sem, &timeoutTs) == -1)
			errExit("sem_timedwait() - Failed to wait for semaphore");
	} else if (sem_wait(sem) == -1)
        errExit("sem_wait");

    printf("%ld sem_wait() succeeded\n", (long) getpid());
    exit(EXIT_SUCCESS);
}

/* Modified by Sergio E. Garcia Tapia for Exercise 30.1 */

/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2023.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 30-1 */

/* thread_incr.c

   This program employs two POSIX threads that increment the same global
   variable, without using any synchronization method. As a consequence,
   updates are sometimes lost.

   See also thread_incr_mutex.c.
*/
#include <pthread.h>
#include "tlpi_hdr.h"

static volatile int glob = 0;   /* "volatile" prevents compiler optimizations
                                   of arithmetic operations on 'glob' */

struct tharg_s {
	char *id;
	int loops;
};

static void *                   /* Loop 'arg->loops' times incrementing 'glob' */
threadFunc(void *arg)
{
	struct tharg_s *threadArgs = (struct tharg_s *) arg;
    int loc, j;

    for (j = 0; j < threadArgs->loops; j++) {
        loc = glob;
        loc++;
        glob = loc;
		printf("[%s]: glob=%d\n", threadArgs->id, glob);
    }

    return NULL;
}

int
main(int argc, char *argv[])
{
    pthread_t t1, t2;
	struct tharg_s pthargs1, pthargs2;
    int loops, s;

    loops = (argc > 1) ? getInt(argv[1], GN_GT_0, "num-loops") : 10000000;
	pthargs1.id = "t1";
	pthargs1.loops = loops;

	pthargs2.id = "t2";
	pthargs2.loops = loops;

    s = pthread_create(&t1, NULL, threadFunc, &pthargs1);
    if (s != 0)
        errExitEN(s, "pthread_create");
    s = pthread_create(&t2, NULL, threadFunc, &pthargs2);
    if (s != 0)
        errExitEN(s, "pthread_create");

    s = pthread_join(t1, NULL);
    if (s != 0)
        errExitEN(s, "pthread_join");
    s = pthread_join(t2, NULL);
    if (s != 0)
        errExitEN(s, "pthread_join");

    printf("glob = %d\n", glob);
    exit(EXIT_SUCCESS);
}

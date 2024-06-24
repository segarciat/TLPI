/* Modified by Sergio E. Garcia Tapia for the purpose of Exercise 39.1 */

/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2023.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 35-2 */

/* sched_set.c

   Usage: sched_set policy priority pid...

   Sets the policy and priority of all process specified by the 'pid' arguments.

   See also sched_view.c.

   The distribution version of this code is slightly different from the code
   shown in the book in order to better fix a bug that was present in the code
   as originally shown in the book. See the erratum for page 743
   (http://man7.org/tlpi/errata/index.html#p_743).
*/
#include <sched.h>
#include <sys/capability.h>
#include "tlpi_hdr.h"

/* 
 * Changes setting of capability incaller's effective capabilities .
 * This is identical to the implementation provided in Listing 39.1 of the textbook.
 */
static int
modifyCap(int capability, int setting)
{
	cap_t caps;
#define CAP_LIST_SIZE 1
	cap_value_t capList[CAP_LIST_SIZE];

	/* Get current capabilities */
	caps = cap_get_proc();
	if (caps == NULL)
		return -1;
	
	/* Change the setting of 'capability' in the effective set of 'caps'. */
	capList[0] = capability;
	if (cap_set_flag(caps, CAP_EFFECTIVE, 1, capList, setting) == -1) {
		cap_free(caps);
		return -1;
	}

	/* Push modified capability sets back to kernel, to change caller's capabilities */
	if (cap_set_proc(caps) == -1) {
		cap_free(caps);
		return -1;
	}

	/* Free the structure that was allocated by libcap */
	if (cap_free(caps) == -1)
		return -1;
	return 0;
}

/* Raise capability in caller's effective set */
static int
raiseCap(int capability)
{
	return modifyCap(capability, CAP_SET);
}

int
main(int argc, char *argv[])
{
    int j, pol;
    struct sched_param sp;

    if (argc < 3 || strchr("rfo"
#ifdef SCHED_BATCH              /* Linux-specific */
                "b"
#endif
#ifdef SCHED_IDLE               /* Linux-specific */
                "i"
#endif
                , argv[1][0]) == NULL)
        usageErr("%s policy priority [pid...]\n"
                "    policy is 'r' (RR), 'f' (FIFO), "
#ifdef SCHED_BATCH              /* Linux-specific */
                "'b' (BATCH), "
#endif
#ifdef SCHED_IDLE               /* Linux-specific */
                "'i' (IDLE), "
#endif
                "or 'o' (OTHER)\n",
                argv[0]);

    pol = (argv[1][0] == 'r') ? SCHED_RR :
                (argv[1][0] == 'f') ? SCHED_FIFO :
#ifdef SCHED_BATCH              /* Linux-specific, since kernel 2.6.16 */
                (argv[1][0] == 'b') ? SCHED_BATCH :
#endif
#ifdef SCHED_IDLE               /* Linux-specific, since kernel 2.6.23 */
                (argv[1][0] == 'i') ? SCHED_IDLE :
#endif
                SCHED_OTHER;

    sp.sched_priority = getInt(argv[2], 0, "priority");

	if (raiseCap(CAP_SYS_NICE) == -1)
		errExit("raiseCap() failed");
    for (j = 3; j < argc; j++)
        if (sched_setscheduler(getLong(argv[j], 0, "pid"), pol, &sp) == -1)
            errExit("sched_setscheduler");

    exit(EXIT_SUCCESS);
}

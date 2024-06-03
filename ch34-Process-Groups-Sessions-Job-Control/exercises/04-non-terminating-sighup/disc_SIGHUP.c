/* Modified by Sergio E. Garcia Tapia for completing Exercise 34.4 */

/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2023.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 34-4 */

/* disc_SIGHUP.c

   This program demonstrates that when a "terminal disconnect" occurs, SIGHUP
   is sent to all the members of the foreground process group for this terminal.

   Try using the following command to run this program in an X-window, and then
   closing the window:

        exec disc_SIGHUP > sig.log 2>&1

   (Since the above will replace the shell with this program, it will be the
   controlling process for the terminal.)
*/
#define _GNU_SOURCE     /* Get strsignal() declaration from <string.h> */
#include <string.h>
#include <signal.h>
#include "tlpi_hdr.h"
#include <limits.h>

static void             /* Handler for SIGHUP */
handler(int sig)
{
    printf("PID %ld: caught signal %2d (%s)\n", (long) getpid(),
            sig, strsignal(sig));
                        /* UNSAFE (see Section 21.1.2) */
}

int
main(int argc, char *argv[])
{
    pid_t parentPid, childPid;
    int j;
    struct sigaction sa;

    if (argc < 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s {d|s}... [ > sig.log 2>&1 ]\n", argv[0]);

    setbuf(stdout, NULL);               /* Make stdout unbuffered */

    parentPid = getpid();
    printf("PID of parent process is:       %ld\n", (long) parentPid);
    printf("Foreground process group ID is: %ld\n",
            (long) tcgetpgrp(STDIN_FILENO));
	sigset_t blockMask;
	sigemptyset(&blockMask);
	sigaddset(&blockMask, SIGRTMIN);
	sigprocmask(SIG_BLOCK, &blockMask, NULL);
    for (j = 1; j < argc; j++) {        /* Create child processes */
        childPid = fork();
        if (childPid == -1)
            errExit("fork");

        if (childPid == 0) {            /* If child... */
            if (argv[j][0] == 'd')      /* 'd' --> to different pgrp */
                if (setpgid(0, 0) == -1)
                    errExit("setpgid");

            sigemptyset(&sa.sa_mask);
            sa.sa_flags = 0;
            sa.sa_handler = handler;
			if (sigaction(SIGHUP, &sa, NULL) == -1)
				errExit("sigaction");

    		printf("PID=%ld PGID=%ld\n", (long) getpid(), (long) getpgrp());

												/* Child notifies parent */
			if (sigqueue(parentPid, SIGRTMIN, (const union sigval) 0) == -1)
				errExit("sigqueue");
            break;                      /* Child exits loop */
        }
    }

    /* All processes fall through to here */
    alarm(60);      /* Ensure each process eventually terminates */

	if (parentPid == getpid()) {		/* Only parent proceeds here */
    	printf("Parent: PID=%ld PGID=%ld\n", (long) getpid(), (long) getpgrp());
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = 0;
		sa.sa_handler = handler;
		if (sigaction(SIGHUP, &sa, NULL) == -1)
			errExit("sigaction parent");
		printf("Suspending until all children have sent SIGRTMIN\n");
		for (int i = 1; i < argc; i++)
			sigwaitinfo(&blockMask, NULL);
		printf("Parent raising SIGHUP\n");
		raise(SIGHUP);
		printf("Parent continuing\n");
	}

	for (;;)
		pause();        /* Wait for signals */
}

#include <sys/types.h> /* pid_t */

#define _XOPEN_SOURCE 500	/* Expose declaration of getsid() in unistd.h */
#include <unistd.h> /* execlp(), fork(), setpgid() */

#include <stdio.h> /* fprintf(), stderr, printf() */
#include <stdlib.h> /* exit(), EXIT_FAILURE, EXIT_SUCCESS */
#include <errno.h> /* errno */
#include <string.h> /* strerror() */

#include <signal.h> /* kill(), sigemptyset(), sigaddset(), sigprocmask(), sigset_t, sigwaitinfo() */

void
unix_error(char *msg)
{
	fprintf(stderr, "%s: %s\n", msg, strerror(errno));
	exit(EXIT_FAILURE);
}

void
displayIDs(char *prefix)
{

	printf("%s: PID (%ld), PPID (%ld), PGID (%ld), SID (%ld)\n",
		prefix, (long) getpid(), (long) getppid(), (long) getpgrp(), (long) getsid(0));
}

int
main(int argc, char *argv[])
{
	pid_t childPid;
	sigset_t blockSet;

	if (sigemptyset(&blockSet) == -1 || sigaddset(&blockSet, SIGUSR1) == -1)
		unix_error("Failed to create block set with SIGUSR1");
	if (sigprocmask(SIG_BLOCK, &blockSet, NULL) == -1) /* Block signal in parent and subsequent children */
		unix_error("Failed to block signals");

	switch(childPid = fork()) {
		case -1:		/* Handle error */
			unix_error("Failed to create child");
		case  0:		/* Child */
			/* Display process group ID and inform parent to proceed */
			displayIDs("Child");

			/* Wait for parent to change process group ID */
			if (argc == 1) {
				printf("Child: About to inform parent to proceed\n");
				if (kill(getppid(), SIGUSR1) == -1)
					unix_error("Child failed to send signal to parent");
				sigwaitinfo(&blockSet, NULL);
				printf("Child resuming\n");
				displayIDs("Child");
			}
			printf("Child about to run exec\n");
			execlp("sleep", "sleep", "5", (char *) NULL);
			unix_error("execlp failed to run sleep");
			/* Run a new program */
		default:		/* Parent */
			/* Displays IDs and wait for SIGUSR1 in pending signals from child */
			displayIDs("Parent");
			printf("Parent pausing until child is ready\n");
			if (argc == 1)
				sigwaitinfo(&blockSet, NULL);
			else
				sleep(1);
			printf("Parent resuming, attempting to set child's process group ID\n");

			/* Change child's process group to a new group of its own */
			if (setpgid(childPid, 0) == -1)
				unix_error("Parent failed to set group ID for child");
			if (argc == 1 && kill(childPid, SIGUSR1) == -1)
				unix_error("Failed to send SIGUSR1 signal to child");
			exit(EXIT_SUCCESS);
	}
}

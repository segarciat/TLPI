#define _XOPEN_SOURCE 500	/* Expose setsid() declaration in unistd.h */
#include <sys/types.h>
#include <unistd.h> /* getpid(), getppid(), getpgrp(), getsid(), setsid(), setpgid() */
#include <stdio.h> /* printf(), fprintf(), stderr */
#include <stdlib.h> /* exit(), EXIT_FAILURE, EXIT_SUCCESS */
#include <errno.h> /* errno */
#include <string.h> /* strerror() */

void
displayIDs(char *prefix)
{
	printf("%s:\n\tPID (%ld), PPID (%ld), PGID (%ld), SID (%ld)\n",
		prefix, (long) getpid(), (long) getppid(), (long) getpgrp(), (long) getsid(0)
	);
}

void
unix_error(char *msg)
{
	fprintf(stderr, "%s: %s\n", msg, strerror(errno));
	exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
	if (getpid() != getpgrp()) {
		displayIDs("Not a process group leader. Becoming one");
		if (setpgid(0, 0) == -1)
			unix_error("Failed to move into new process group");
	}
	
	displayIDs("Currently process group leader");
	printf("About to try create a new session and become the session leader\n");
	if (setsid() == -1)
		unix_error("Failed to create new session");
	exit(EXIT_SUCCESS); /* Should never reach this point */
}

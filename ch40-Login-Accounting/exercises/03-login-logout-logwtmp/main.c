#include "myutmp.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>

int
main(int argc, char *argv[])
{
	struct utmp ut;
	char *terminalName;

	printf("pid: %ld\n", (long) getpid());

	/* Clear the structure populate entry with timestamp and name */
	memset(&ut, 0, sizeof(struct utmp));
	ut.ut_tv.tv_sec = time(NULL);
	strncpy(ut.ut_user, "sgarciat", sizeof(ut.ut_user));

	/* Write entry to utmp and wtmp */
	errno = 0;
	_login(&ut);
	if (errno != 0)
		fprintf(stderr, "%s: Error during _login (%s)\n", argv[0], strerror(errno));

	/* Pause long enough to allow verifying with who(1) and last(1) commands */
	printf("Sleeping for %d seconds before logout\n", 10);
	sleep(10);

	/* Get terminal device name associated with standard input */
	if ((terminalName = ttyname(STDIN_FILENO)) == NULL)
		perror(argv[0]);
	
	/* Attempt to update utmp entry */
	if (!_logout(terminalName + 5)) /* skip /dev/ at start of terminal name */
		perror(argv[0]);
	
	if (errno != 0)
		exit(EXIT_FAILURE);
	exit(EXIT_SUCCESS);
}

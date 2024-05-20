#include <unistd.h> /* fork(), vfork(), _exit() */
#include <sys/wait.h> /* wait() */
#include <stdlib.h> /* strtol(), exit() EXIT_FAILURE, EXIT_SUCCESS */
#include <errno.h> /* errno */
#include <string.h> /* strerror(), strcmp() */
#include <stdio.h> /* fprintf(), stderr */

#define RUN_COUNT_ARG_INDEX 1

static void
errorExit(char *msg)
{
	fprintf(stderr, "%s: %s\n", msg, errno == 0 ? "" : strerror(errno));
	exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
	if (argc != 2 || strcmp("--help", argv[1]) == 0) {
		fprintf(stderr, "Usage: %s number-of-runs\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* Parse number of runs */
	char *endp;
	errno = 0;
	long runs = strtol(argv[RUN_COUNT_ARG_INDEX], &endp, 0);
	if (errno == ERANGE)
		errorExit("Number is too large or too small");
	if (*endp != '\0') {
		fprintf(stderr, "Bad character %c in %s\n", *endp, argv[RUN_COUNT_ARG_INDEX]);
		exit(EXIT_FAILURE);
	} else if (runs <= 0) {
		fprintf(stderr, "Number of runs must be positive, but got: %ld\n", runs);
		exit(EXIT_FAILURE);
	}

	/* Begins test */
	for (long r = 0; r < runs; r++) {
		switch (
#ifndef VFORK
			fork()
#else
			vfork()
#endif
		) {
			case -1:
				fprintf(stderr, "Failed on run %ld\n", r);
				errorExit("Fork failed");
			case  0:
				_exit(EXIT_SUCCESS);
				break;
			default:
				if (wait(NULL) == -1)
					errorExit("wait failed");
				break;
		}
	}
	exit(EXIT_SUCCESS);
}

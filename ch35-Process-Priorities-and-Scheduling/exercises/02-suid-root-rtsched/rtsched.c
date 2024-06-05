#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

void
unix_error(char *msg)
{
	fprintf(stderr, "%s: %s\n", msg, strerror(errno));
	exit(EXIT_FAILURE);
}

#define POLICY_ARG_IDX 1
#define PRIORITY_ARG_IDX 2
#define COMMAND_ARG_IDX 3

int
main(int argc, char *argv[])
{
	if (argc < 4) {
		fprintf(stderr, "Usage: %s policy priority command [args...]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* Get desired policy */
	char *endp;
	int policy;
	struct sched_param schedParam;

	if (strcmp("r", argv[POLICY_ARG_IDX]) == 0)
		policy = SCHED_RR;
	else if (strcmp("f", argv[POLICY_ARG_IDX]) == 0)
		policy = SCHED_FIFO;
	else {
		fprintf(stderr, "Invalid realtime policy %s. "
				"Must be %c for SCHED_RR or %c for SCHED_FIFO\n",
					argv[POLICY_ARG_IDX], 'r', 'f');
		exit(EXIT_FAILURE);
	}

	/* Get priority */
	errno = 0;
	schedParam.sched_priority = strtol(argv[PRIORITY_ARG_IDX], &endp, 0);
	if (errno != 0) {
		fprintf(stderr, "Error parsing priority: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	} else if (*endp != '\0') {
		fprintf(stderr, "Expected non-numeric character '%c' in string %s\n", *endp, argv[PRIORITY_ARG_IDX]);
		exit(EXIT_FAILURE);
	}

	/* Set realtime policy and priority */
	if (sched_setscheduler(0, policy, &schedParam) == -1)
		unix_error("Failed to set realtime scheduling policy");
	
	/* Give up privileges */
	if (setuid(getuid()) == -1)
		unix_error("Failed to set user ID");

	/* Run given command */
	if (execvp(argv[COMMAND_ARG_IDX], argv + COMMAND_ARG_IDX) == -1) {
		fprintf(stderr, "Failed to execute given %s command: %s\n", argv[COMMAND_ARG_IDX], strerror(errno));
		exit(EXIT_FAILURE);
	}
}

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/resource.h>
#include <sys/wait.h>

#define CMD_ARG_IDX 1
#define MICROS_PER_SEC 1000000

void
unix_error(char *msg)
{
	fprintf(stderr, "%s: %s\n", msg, strerror(errno));
	exit(EXIT_FAILURE);
}

void
displayResourceUsage(struct rusage ru)
{
	printf("\nUser CPU time: %.6fs\n"
			"System CPU time: %.6fs\n",
			ru.ru_utime.tv_sec + ((ru.ru_utime.tv_usec + 0.0) / MICROS_PER_SEC),
			ru.ru_stime.tv_sec + ((ru.ru_stime.tv_usec + 0.0) / MICROS_PER_SEC)
	);
}

int
main(int argc, char *argv[])
{
	pid_t childPid;
	struct rusage resourceUsage;

	if (argc < 2 || strcmp("--help", argv[1]) == 0) {
		fprintf(stderr, "Usage: %s command [arg]...\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	switch (childPid = fork()) {
		case -1: unix_error("Failed to fork a new child");
		case  0:
			if (execvp(argv[CMD_ARG_IDX], argv + CMD_ARG_IDX) == -1)
				unix_error("Failed to exec program");
		default:
			if (waitpid(childPid, NULL, 0) == -1)
				unix_error("Failed to reap child");
			if (getrusage(RUSAGE_CHILDREN, &resourceUsage) == -1)
				unix_error("Failed to get resource usage for child");
			displayResourceUsage(resourceUsage);
			exit(EXIT_SUCCESS);
	}
}

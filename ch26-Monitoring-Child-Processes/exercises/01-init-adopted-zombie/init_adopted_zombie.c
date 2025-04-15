#define _DEFAULT_SOURCE
#include <sys/types.h>  /* pid_t */
#include <unistd.h>     /* fork() */
#include <stdlib.h>     /* exit(), EXIT_FAILURE */
#include <errno.h>      /* errno */
#include <stdio.h>      /* fprintf(), printf(), stderr, stdout */
#include <string.h>     /* strerror() */
#include <limits.h>     /* PATH_MAX */

#define CHILD_SLEEP 2
#define PARENT_SLEEP 1

void
error(const char *msg)
{
	fprintf(stderr, "%s: %s\n", msg, strerror(errno));
	exit(EXIT_FAILURE);
}

int
main()
{
	pid_t pid;

	/* Don't buffer output */
	setbuf(stdout, NULL);
	switch(pid = fork()) {
		case -1:
            error("Fork failed");
            break;
		case  0:; /* Child (intentional null statement) */
			/* Display parent before sleep */
			pid_t childPid = getpid();
			printf("Child (%ld): Parent has pid %ld. Sleeping...\n", (long) childPid, (long) getppid());
			sleep(CHILD_SLEEP);
			/* Display parent after sleep */
			pid_t newParent = getppid();
			printf("Child (%ld): Awake! Parent has pid %ld. Searching for parent process name\n",
					(long) childPid, (long) newParent);

			/* Find parent process name in /proc filesystem */
			char parentPidFilename[PATH_MAX]; /* Includes terminating null byte */
			snprintf(parentPidFilename, PATH_MAX, "/proc/%ld/status", (long) newParent);
			FILE *fp = fopen(parentPidFilename, "r");
			if (fp == NULL)
				error("Failed to find parent proc file");
			char line[BUFSIZ];
			while (fgets(line, BUFSIZ, fp) != NULL) {
				if (strstr(line, "Name") != NULL) {
					printf("Found! %s\n", line);
					exit(EXIT_SUCCESS);
				}
			}
			if (!feof(fp))
				error("Error reading parent process file");
			printf("Failed to find parent");
			exit(EXIT_FAILURE);
		default:; /* Parent (intentional null statement) */
			pid_t parentPid = getpid();
			printf("Parent (%ld): Sleeping...\n", (long) parentPid);
			sleep(PARENT_SLEEP);
			printf("Parent (%ld): Exiting\n", (long) parentPid);
			exit(EXIT_SUCCESS);
	}
}

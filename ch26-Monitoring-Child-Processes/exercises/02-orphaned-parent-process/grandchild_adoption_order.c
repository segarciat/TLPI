#define _DEFAULT_SOURCE
#include <sys/types.h> /* pid_t */
#include <unistd.h> /* fork(), sleep() */
#include <sys/wait.h> /* wait() */
#include <stdio.h> /* printf(), fprintf(), setbuf(), stderr, stdout */
#include <stdlib.h> /* exit(), EXIT_SUCCESS, EXIT_FAILURE */
#include <errno.h> /* errno */
#include <string.h> /* strerror() */

#define CHILD_SLEEP 2
#define GRANDPARENT_SLEEP 4
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

	/* Disable buffering */
	setbuf(stdout, NULL);

	switch(pid = fork()) {
		case -1:
            error("Fork failed in grandparent");
            break;
		case  0: /* Parent */
			printf("Parent (%ld): Creating child...\n", (long) getpid());
			switch(pid = fork()) {
				case -1:
                    error("Fork failed in parent");
                    break;
				case  0:; /* Child */
					pid_t childPid = getpid();
					printf("Child (%ld): Child is born! Parent is %ld. Sleeping...\n",
						(long) childPid, (long) getppid());
					sleep(CHILD_SLEEP);
					printf("Child (%ld): Child awakes. Parent is now %ld. Sleeping again...\n",
							(long) childPid, (long) getppid());
					sleep(CHILD_SLEEP);
					printf("Child (%ld): Child awakes again. Parent is now %ld. Exiting\n",
							(long) childPid, (long) getppid());
					exit(EXIT_SUCCESS);
				default:; /* Parent */
					pid_t parentPid = getpid();
					printf("Parent (%ld): Sleeping...\n", (long) parentPid);
					sleep(PARENT_SLEEP);
					printf("Parent (%ld): Awake! Exiting.\n", (long) parentPid);
					exit(EXIT_SUCCESS);
			}
            break;
		default:; /* Grandparent */
			pid_t grandparentPid = getpid();
			printf("Grandparent (%ld): Sleeping...\n", (long) grandparentPid);
			sleep(GRANDPARENT_SLEEP);
			printf("Grandparent (%ld): Awake! Reaping child...\n", (long) grandparentPid);
			if (wait(NULL) == -1)
				error("Error waiting for child");
			printf("Grandparent (%ld): Reaped child. Sleeping\n", (long) grandparentPid);
			sleep(GRANDPARENT_SLEEP);
			printf("Grandparent (%ld): Exiting.\n", (long) grandparentPid);
			exit(EXIT_SUCCESS);
	}
}

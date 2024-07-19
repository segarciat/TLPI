#include "binary_sems.h"
#include <sys/stat.h> /* Permission flags - S_IRUSR, S_IWUSR */
#include "semun.h"	 /* union semun */
#include <unistd.h> /* fork(), _exit() */
#include <stdio.h> /* printf(), fprintf(), stderr, stdout, setbuf, NULL */
#include <stdlib.h> /* exit(), EXIT_FAILURE, EXIT_SUCCESS */
#include <sys/wait.h> /* waitpid() */

static int semId;

/* Exit handler to remove semaphore set created by this program on exit */
void
removeSemaphore()
{
	union semun arg;
	if (semctl(semId, /* Unused */ 0, IPC_RMID, /* Unused */ arg) == -1)
		fprintf(stderr, "[%ld]: Failed to remove semaphore with ID %ld\n", (long) getpid(), (long) semId);
	printf("[%ld] Deleted semaphore with ID %ld\n", (long) getpid(), (long) semId);
}

/* Demonstrates the effectiveness of a nonblocking attempt to reserve a binary semaphore */

int
main(int argc, char *argv[])
{
	pid_t childPid;

	/* Create semaphore */
	semId = semget(IPC_PRIVATE, /* One semaphore */ 1, S_IRUSR | S_IWUSR);
	if (semId == -1)
		errExit("semget() -  Failed to create semaphore");
	printf("[%ld] Parent created semaphore with ID %ld\n", (long) getpid(), (long) semId);
	if (atexit(removeSemaphore) != 0)
		errExit("Failed to set exit handler for removing semaphore");

	/* Initialize semaphore */
	if (initSemInUse(semId, /* Sem num */ 0) == -1)
		errExit("Failed to initialize semaphore 0 in semaphore set with ID %ld", (long) semId);
	/* Fork */
	setbuf(stdout, NULL);	/* Disable buffering */
	switch (childPid = fork()) {
		case -1: errExit("fork() failed"); /* error */
		case  0: /* Child */
			/* Attempt to reserve in child and fail */
			printf("[%ld] Child about to attempt to reserve semaphore\n", (long) getpid());
			if (reserveSemNB(semId, /* Sem num */ 0) == -1 && errno != EAGAIN)
				errExit("Unexpected error during nonblocking attempt to re serve semaphore");
			printf("[%ld] Child: As expected, failed to reserve semaphore; already in-use\n", (long) getpid());
			_exit(EXIT_SUCCESS);
		default: /* Parent */
			printf("[%ld] Parent about to wait for child to exit\n", (long) getpid());
			while (waitpid(childPid, NULL, 0) == -1)
				if (errno != EINTR)
					errExit("Failed to reap child process whose ID is %ld", (long) childPid);
			printf("[%ld] Parent reaped child, exiting\n", (long) getpid());
			exit(EXIT_SUCCESS);
	}
}

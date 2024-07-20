#include "fifo_binary_sem.h"
#include <stdio.h> /* printf(), fprintf(), stderr */
#include <stdlib.h> /* exit(), EXIT_SUCCESS, EXIT_FAILURE */
#include <unistd.h> /* fork(), getpid() */
#include <errno.h> /* errno */
#include <string.h> /* strrerror() */
#include <sys/stat.h> /* mkfifo(), S_IRUSR, S_IWUSR */
#include <sys/wait.h>

#define FIFO_PATH "./binary_semaphore.fifo"

void
unix_error(char *msg)
{
	fprintf(stderr, "%s: %s\n", msg, strerror(errno));
	exit(EXIT_FAILURE);
}

void
removeFifo()
{
	pid_t pid = getpid();
	if (unlink(FIFO_PATH) == -1)
		fprintf(stderr, "[%ld] Failed to destroy FIFO: %s\n", (long) pid, FIFO_PATH);
	printf("[%ld] Successfully removed FIFO: %s\n", (long) pid, FIFO_PATH);
}

void
useSemaphore()
{
	pid_t pid;

	pid = getpid();
	if (reserveSemNB(FIFO_PATH) == -1) {
		if (errno != EAGAIN)
			unix_error("Error reserving semaphorE");
		printf("[%ld] Semaphore already in-use, waiting to use...\n", (long) pid);
		if (reserveSem(FIFO_PATH) == -1)
			unix_error("Error reserving semaphore");
		printf("[%ld] Acquired semaphore on second attempt!\n", (long) pid);
	} else {
		printf("[%ld] Semaphore was immediately available! Sleeping for a second...\n", (long) pid);
		sleep(1);
		printf("[%ld] Done sleeping.\n", (long) pid);
	}
	printf("[%ld] Ok, releasing semaphore\n", (long) pid);
	if (releaseSem(FIFO_PATH) == -1)
		unix_error("Error releasing semaphore");
}

int
main(int argc, char *argv[])
{
	struct sempipe sp;
	int count = 0;

	/* Create FIFO */
	if (mkfifo(FIFO_PATH, S_IRUSR | S_IWUSR) == -1 && errno != EEXIST)
		unix_error("Failed to create FIFO");
	printf("FIFO created: %s\n", FIFO_PATH);

	/* Set FIFO to be destroyed on exit */
	if (atexit(removeFifo) == -1)
		unix_error("Failed to set exit handler to remove FIFO");
	
	/* Initialize FIFO as a semaphore */
	if (initSemAvailable(FIFO_PATH, &sp) == -1)
		unix_error("Failed to initialize FIFO for use as a semaphore");

	setbuf(stdout, NULL); /* Disable stdio buffering for stdout */
	switch (fork()) {
		case -1: unix_error("Failed to fork child");
		case  0: /* Child */
			useSemaphore();
			_exit(EXIT_SUCCESS);
		default: // Parent
			useSemaphore();
			printf("[%ld] Waiting on child...\n", (long) getpid());
			while (wait(NULL) == -1)
				if (errno != EINTR)
					unix_error("Error while reading child");
			printf("[%ld] Reaped child. Parent disabling semaphore.\n", (long) getpid());
			if (closeSem(&sp) == -1)
				unix_error("Parent failed to disable FIFO");
			exit(EXIT_SUCCESS);
	}
}

#define _DEFAULT_SOURCE /* Enable sigaction */
#include <stdio.h>      /* printf() */
#include <unistd.h>     /* sleep() */
#include <signal.h>     /* sigset_t, sigemptyset(), sigaddset(), SIGABRT, sigprocmask(), SIG_BLOCK */
#include <stdlib.h>     /* exit(), EXIT_FAILURE, EXIT_SUCCESS */
#include "abort.h"      /* s_abort() */

static void
abortHandler(int sig)
{
    (void) sig;
	/* UNSAFE: printf is a non-async-signal-safe function */
	printf("Abort handler called\n");
}

int
main(int argc, char *argv[])
{
    (void) argc;
	printf("Setting abort handler\n");
	struct sigaction sa;
	if (sigemptyset(&sa.sa_mask) == -1) {
		fprintf(stderr, "%s: Failed to create empty process mask for SIGABRT handler\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	sa.sa_flags = 0;
	sa.sa_handler = abortHandler;
	if (sigaction(SIGABRT, &sa, NULL) == -1) {
		fprintf(stderr, "%s: Failed to set disposition for SIGABRT\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	printf("Adding abort to signal mask\n");
	sigset_t blockSet;
	sigemptyset(&blockSet);
	sigaddset(&blockSet, SIGABRT);
	if (sigprocmask(SIG_BLOCK, &blockSet, NULL) == -1) {
		fprintf(stderr, "%s: Failed to add SIGABRT to process signal mask\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	while (1) {
		printf("Sleeping and then trying to abort...\n");
		sleep(3);
		s_abort();
	}
	exit(EXIT_SUCCESS);
}

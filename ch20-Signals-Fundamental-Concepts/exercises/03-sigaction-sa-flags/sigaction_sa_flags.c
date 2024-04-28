#include <signal.h> /* struct sigaction, sigaction(), SIGINT, SA_NODEFER, SA_RESETHAND */
#include <stdlib.h> /* exit(), EXIT_FAILURE */
#include <stdio.h> /* fprintf(), stderr */ 
#define _GNU_SOURE
#include <string.h> /* strsignal() */
#include <unistd.h> /* sleep() */

void handlerNoDefer(int signal)
{
	printf("Caught %s! Sleeping for 5 seconds...unless you interrupt\n", strsignal(signal));
	fflush(stdout);
	unsigned int remaining = sleep(5);
	if (remaining > 0) {
		printf("I could have slept for %u more seconds...\n", remaining);
	}
	printf("Done!\n");
}

void handlerResetHand(int signal)
{
	printf("Caught %s! Resetting disposition\n", strsignal(signal));
}

int
main(int argc, char *argv[])
{
	struct sigaction sa;
	sa.sa_flags = SA_NODEFER;
	sa.sa_handler = handlerNoDefer;
	int signal = SIGINT;
	char *signalDescription = strsignal(signal);
	if (sigaction(signal, &sa, NULL) == -1) {
		fprintf(stderr, "%s: Failed to disposition for %s signal\n", argv[0], signalDescription);
		exit(EXIT_FAILURE);
	}
	printf("Set disposition on %s signal with SA_NODEFER, allowing recursively calling handler\n", signalDescription);
	pause();
	sa.sa_flags = SA_RESETHAND;
	sa.sa_handler = handlerResetHand;
	if (sigaction(signal, &sa, NULL) == -1) {
		fprintf(stderr, "%s: Failed to disposition for %s signal\n", argv[0], signalDescription);
		exit(EXIT_FAILURE);
	}
	printf("Set disposition on %s with SA_RESETHAND, resetting after it's done\n", signalDescription);
	pause();
	printf("Ok go ahead and interrupt now\n");
	fflush(stdout);
	pause();
}

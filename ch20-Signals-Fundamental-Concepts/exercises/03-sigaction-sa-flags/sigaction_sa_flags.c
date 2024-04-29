#include <signal.h> /* struct sigaction, sigaction(), SIGINT, SA_NODEFER, SA_RESETHAND */
#include <stdlib.h> /* exit(), EXIT_FAILURE */
#include <stdio.h> /* fprintf(), stderr */ 
#define _GNU_SOURE
#include <string.h> /* strsignal() */
#include <unistd.h> /* sleep() */

void handlerNoDefer(int signal)
{
	/* UNSAFE: Handler uses stdio.h functions that are non-async-signal-safe */
	printf("Caught %s! Sleeping for 5 seconds...unless you interrupt\n", strsignal(signal));
	fflush(stdout);

	unsigned int remaining = sleep(5);
	if (remaining > 0)
		printf("I could have slept for %u more seconds...\n", remaining);
	printf("Done!\n");
}

void handlerResetHand(int signal)
{
	printf("Caught %s! Resetting disposition. Try again\n", strsignal(signal));
}

int
main(int argc, char *argv[])
{
	int signal = SIGINT;
	/* Set disposition for SIGINT with SA_NODEFER, which means it won't be added to
	 * the process signal mask when handled */
	struct sigaction sanodefer, saresethand;
	if (sigemptyset(&sanodefer.sa_mask) == -1) {
		fprintf(stderr, "%s: Failed to set mask for sigaction\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	sanodefer.sa_flags = SA_NODEFER;
	sanodefer.sa_handler = handlerNoDefer;
	char *signalDescription = strsignal(signal);
	if (sigaction(signal, &sanodefer, NULL) == -1) {
		fprintf(stderr, "%s: Failed to disposition for %s signal\n", argv[0], signalDescription);
		exit(EXIT_FAILURE);
	}

	/* Wait for a signal to be handled before proceeding */
	printf("Set disposition on %s signal with SA_NODEFER, allowing recursively calling handler\n", signalDescription);
	pause();

	/* Now set SA_RESETHAND, so that the disposition is reset after the first time it is handled */
	if (sigemptyset(&saresethand.sa_mask) == -1) {
		fprintf(stderr, "%s: Failed to set mask for sigaction\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	saresethand.sa_flags = SA_RESETHAND;
	saresethand.sa_handler = handlerResetHand;
	if (sigaction(signal, &saresethand, NULL) == -1) {
		fprintf(stderr, "%s: Failed to disposition for %s signal\n", argv[0], signalDescription);
		exit(EXIT_FAILURE);
	}

	/* Pause for until process receives SIGINT */
	printf("Set disposition on %s with SA_RESETHAND, resetting after it's done\n", signalDescription);
	pause();

	/* Inform user that the handler default will work as normal now */
	printf("Ok go ahead and interrupt now\n");
	fflush(stdout);
	pause();

	exit(EXIT_SUCCESS);
}

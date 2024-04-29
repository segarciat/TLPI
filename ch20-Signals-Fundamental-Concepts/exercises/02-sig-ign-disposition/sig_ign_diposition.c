#define _GNU_SOURCE /* for strsignal() */
#include <string.h>
#include <signal.h> /* sigaction(), struct sigaction, SIG_IGN, SIG_DFL, sigemptyset() */
#include <stdio.h> /* fprintf(), stderr */
#include <stdlib.h> /* exit(), EXIT_FAILURE */
#include <unistd.h> /* sleep() */

int
main(int argc, char *argv[])
{
	/* Set disposition of SIGTERM to ignore */
	struct sigaction sa;
	if (sigemptyset(&sa.sa_mask) == -1) {
		fprintf(stderr, "%s: Failed to set mask for signal action\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	sa.sa_handler = SIG_IGN;
	sa.sa_flags = 0;
	int signal = SIGINT;
	char *signalDescription = strsignal(signal);
	if (sigaction(signal, &sa, NULL) == -1) {
		fprintf(stderr, "%s: Failed to set disposition of %s\n", argv[0], signalDescription);
		exit(EXIT_FAILURE);
	}

	/* Idle process for a short duration */
	unsigned int duration = 5;
	printf("%s ignored. Sleeping for about %u seconds...\n", signalDescription, duration);
	fflush(stdout);
	sleep(duration);

	/* Reset the signal disposition */
	printf("Done sleeping! Resetting disposition of %s\n", signalDescription);
	sa.sa_handler = SIG_DFL;
	if (sigaction(signal, &sa, NULL) == -1) {
		fprintf(stderr, "%s: Failed to set disposition of %s\n", argv[0], signalDescription);
		exit(EXIT_FAILURE);
	}

	/* Suspend until user interrupts */
	printf("Pausing until interrupted... (Press CTRL+C)\n");
	pause();
	exit(EXIT_SUCCESS);
}

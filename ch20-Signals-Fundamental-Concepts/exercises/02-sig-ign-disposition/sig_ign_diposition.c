#define _GNU_SOURCE /* for strsignal() */
#include <string.h>
#include <signal.h> /* sigaction(), struct sigaction, SIG_IGN */
#include <stdio.h> /* fprintf(), stderr */
#include <stdlib.h> /* exit(), EXIT_FAILURE */
#include <unistd.h> /* sleep() */

int
main(int argc, char *argv[])
{
	/* Set disposition of SIGTERM to ignore */
	struct sigaction sa;
	int signal = SIGINT;
	sa.sa_handler = SIG_IGN;
	sa.sa_flags = 0;
	char *signalDescription = strsignal(signal);
	if (sigaction(signal, &sa, NULL) == -1) {
		fprintf(stderr, "%s: Failed to set disposition of %s\n", argv[0], signalDescription);
		exit(EXIT_FAILURE);
	}

	unsigned int duration = 5;
	printf("%s ignored. Sleeping for about %u seconds...\n", signalDescription, duration);
	fflush(stdout);
	sleep(duration);
	printf("Done sleeping! Resetting disposition of %s\n", signalDescription);
	sa.sa_handler = SIG_DFL;
	if (sigaction(signal, &sa, NULL) == -1) {
		fprintf(stderr, "%s: Failed to set disposition of %s\n", argv[0], signalDescription);
		exit(EXIT_FAILURE);
	}
	printf("Pausing until interrupted... (Press CTRL+C)\n");
	pause();
	exit(EXIT_SUCCESS);
}

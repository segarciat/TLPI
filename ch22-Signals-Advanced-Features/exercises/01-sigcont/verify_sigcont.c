#include <stdio.h> /* fprintf(), stderr, printf(), NULL */
#include <signal.h> /* struct sigaction, sigemptyset(), sigaction(), SIGCONT, sigaddset(), sigset_t */
#include <stdlib.h> /* exit(), EXIT_FAILURE */
#include <unistd.h> /* pause(), getpid() */

static void
errAndExit(char *msg)
{
	fprintf(stderr, "%s\n", msg);
	exit(EXIT_FAILURE);
}

static void
sigcontHandler(int signal)
{
	/* UNSAFE: printf() is a non-async-signal-safe function */
	printf("Handled SIGCONT\n");
}

int
main(int argc, char *argv[])
{
	/* Establish disposition (handler) for SIGCONT */
	struct sigaction sa;
	
	if (sigemptyset(&sa.sa_mask) == -1)
		errAndExit("Failed to set signal mask for SIGCONT handler");
	sa.sa_flags = 0;
	sa.sa_handler = sigcontHandler;
	if (sigaction(SIGCONT, &sa, NULL) == -1)
		errAndExit("Failed to change SIGCONT disposition: handler not established");

	/* Add SIGCONT to process signal mask */
	sigset_t blockSet;
	if (sigemptyset(&blockSet) == -1)
		errAndExit("Failed to initialize signal block set");
	if (sigaddset(&blockSet, SIGCONT) == -1)
		errAndExit("Failed to add SIGCONT to block set");
	if (sigprocmask(SIG_BLOCK, &blockSet, NULL) == -1)
		errAndExit("Failed to set disposition for SIGCONT");

	printf("PID: %ld\n", (long) getpid());
	printf("Blocking and handling SIGCONT\n");
	int remaining = 10;
	printf("Unblocking SIGCONT in: %d\n", remaining);
	while (remaining > 0) {
		sleep(1);
		printf("%d\n", --remaining);
	}
	if (sigprocmask(SIG_UNBLOCK, &blockSet, NULL) == -1)
		errAndExit("Failed to unblock SIGCONT");
	printf("Exiting\n");
	
	exit(EXIT_SUCCESS);
}

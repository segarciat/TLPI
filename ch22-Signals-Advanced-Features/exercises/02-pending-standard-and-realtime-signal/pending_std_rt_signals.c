#define _DEFAULT_SOURCE /* Expose signal interface */
#include <stdio.h>      /* fprintf(), stderr */
#include <stdlib.h>     /* exit(), EXIT_FAILURE */
#include <string.h>     /* strsignal() */
#include <signal.h>     /* struct sigaction, sigaction(), NSIG */
#include <unistd.h>     /* sleep() */
#include "tlpi_hdr.h"

static void
handleAllSignals(int sig, siginfo_t *si, void *ucontext)
{
    (void) si;
    (void) ucontext;
	/* UNSAFE: printf() is a non-async-signal-safe function */
	printf("Received: (%d) %s\n", sig, strsignal(sig));
}

int
main(int argc, char *argv[])
{
    (void) argc;
	printf("%s: PID: %ld\n", argv[0], (long) getpid());
	struct sigaction sa;
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = handleAllSignals;
	if (sigfillset(&sa.sa_mask) == -1)
		errExit("%s: Failed to set signal mask for signal handler", argv[0]);
	/* Set disposition for all signals */
	for (int sig = 1; sig < NSIG; sig++)
		if (sig != SIGTSTP && sig != SIGINT)
			(void) sigaction(sig, &sa, NULL); /* Ignore errors, particularly from nonexistent signals */
	
	/* Add all signals to signal mask */
	sigset_t blockMask, prevMask;
	if (sigfillset(&blockMask) == -1)
		errExit("%s: Failed to fill block set for signals", argv[0]);
	sigdelset(&blockMask, SIGTSTP);
	sigdelset(&blockMask, SIGINT);
	if (sigprocmask(SIG_SETMASK, &blockMask, &prevMask) == -1)
		errExit("%s: Failed to set process signal mask", argv[0]);

	/* Sleep for a short duration */
	printf("%s: All signals but the following are blocked: \n", argv[0]);
	for (int sig = 1; sig < NSIG; sig++)
		if (sigismember(&blockMask, sig) == 0)
			printf("\t(%2d) %s\n", sig, strsignal(sig));
	unsigned remaining = 30;
	printf("\n%s: Sleeping up to %u seconds, then unblocking.\n", argv[0], remaining);
	fflush(stdout);
	sleep(remaining);

	/* Unblock all signals */
	printf("%s: Unblocking signals...\n", argv[0]);
	if (sigprocmask(SIG_SETMASK, &prevMask, NULL) == -1)
		errExit("%s: Failed to restore process signal mask", argv[0]);
	printf("%s: Exiting\n", argv[0]);
	exit(EXIT_SUCCESS);
}

#include <signal.h> /* raise(), SIGABRT, SIG_BLOCK, SIG_DFL, sigdelset(), sigprocmask() */
#include <stddef.h> /* NULL */
#include <stdio.h> /* fflush(), stdin, stdout, stderr, fclose() */

void s_abort(void)
{
	/* Remove SIGABRT from process signal mask so that it won't be blocked */
	sigset_t unblockSet;
	sigemptyset(&unblockSet);
	sigaddset(&unblockSet, SIGABRT);
	sigprocmask(SIG_UNBLOCK, &unblockSet, NULL);

	/* Obtain old disposition */
	struct sigaction oldact;
	sigaction(SIGABRT, NULL, &oldact);

	/* Allow programmer-defined handler that returns */
	if (oldact.sa_handler != SIG_DFL && oldact.sa_handler != SIG_IGN)
		raise(SIGABRT);

	/* Reset disposition */
	oldact.sa_handler = SIG_DFL;
	sigaction(SIGABRT, &oldact, NULL);

	/* Flush all stdio buffers associated with output streams */
	fflush(NULL);

	/* Close stdio streams */
	fclose(stdin);
	fclose(stdout);
	fclose(stderr);

	/* Abort, terminating and dumping core */
	raise(SIGABRT);
}

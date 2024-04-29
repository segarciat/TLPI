#include <signal.h> /* NSIG, struct sigaction, sigaction(), SA_RESTART */
#include <stddef.h> /* NULL */


/*
 * When flag is 1, causes a handler for signal sig to interrupt blocking system calls.
 * When flag is 0, causes blocking system calls to be restarted after handler executes for signal sig.
 * 
 * Returns 0 on success and -1 on error.
 */
int
_siginterrupt(int sig, int flag)
{
	if (sig == 0 || sig >= NSIG || (flag & ~1)) /* Flag is not 0 nor 1 */
		return -1;

	/* Get current disposition */
	struct sigaction oldact;
	if (sigaction(sig, NULL, &oldact) == -1)
		return -1;
	
	/* Update interrupt behavior for blocking system calls */
	if (flag)
		oldact.sa_flags &= ~SA_RESTART;
	else
		oldact.sa_flags |=  SA_RESTART;
	
	/* Update disposition */
	if (sigaction(sig, &oldact, NULL) == -1)
		return -1;
	0;
}

#include <signal.h> /* NSIG, struct sigaction, sigaction(), SA_RESTART */
#include <stddef.h> /* NULL */

/*
 * When flag is 1, causes a handler for signal sig to interrupt blocking system calls.
 * When flag is 0, causes blocking system calls to be restarted after execution of a handler for sig.
 * 
 * Returns 0 on success and -1 on error.
 */
int
my_siginterrupt(int sig, int flag)
{
    /* Flag is not 0 nor 1 */
    if (sig <= 0 || sig >= NSIG || (flag & ~1)) 
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
    return 0;
}

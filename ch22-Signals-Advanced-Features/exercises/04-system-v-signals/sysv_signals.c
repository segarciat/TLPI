#define _XOPEN_SOURCE 500 /* Expose SIG_HOLD when including signal.h */
#include <signal.h>
#include <stddef.h> /* NULL */

typedef void (*sysv_sighandler_t)(int);

/*
 * If handler is SIGHOLD, adds sig to the process signal mask, and leaves the disposition
 * unchanged.
 *
 * If handler is anything other than SIGHOLD, then it removes sig from the process signal
 * mask. Additionally, it sets the given handler as the new disposition. sig will be blocked
 * during the execution of handler. When handler is done executing, the signal will be
 * unblocked, and any blocking system calls that were interrupted will be restarted.
 * 
 * If sig was already in the process signal mask, returns SIG_HOLD. Otherwise, returns
 * the previous disposition.

 * On error, returns -1.
 *
 * See sigset (3) for more details.
 */
sysv_sighandler_t sigset(int sig, sysv_sighandler_t handler)
{
	sigset_t prevMask;
	struct sigaction oldsa, newsa;
	sysv_sighandler_t error = (sysv_sighandler_t) -1;

	newsa.sa_flags = SA_RESTART;
	newsa.sa_handler = handler;
	sigemptyset(&newsa.sa_mask);
	if (sigaddset(&newsa.sa_mask, sig) == -1)
		return error; /* Invalid signal */
	
	/* Signal is valid */
	int maskAction = (handler == SIG_HOLD) ? SIG_BLOCK : SIG_UNBLOCK;
	if (sigprocmask(maskAction, &newsa.sa_mask, &prevMask) == -1)
		return error;

	int wasBlocked = sigismember(&prevMask, sig);
	if (handler != SIG_HOLD || wasBlocked)
		if (sigaction(sig, &newsa, &oldsa) == -1)
			return error;

	switch(wasBlocked) {
		case  0  : return oldsa.sa_handler;
		case  1  : return SIG_HOLD;
		case -1  :
		default  : return error;
	}
}

/* Adds sig to process signal mask.
 * Returns 0 on success, or -1 on failure
 */
int sysv_sighold(int sig)
{
	sigset_t sigMask;
	sigemptyset(&sigMask);
	if (sigaddset(&sigMask, sig) == -1 || sigprocmask(SIG_BLOCK, &sigMask, NULL) == -1)
		return -1;
	return 0;
}

/* Remove sig from process signal mask.
 * Returns 0 on success, or -1 on failure
 */
int sysv_sigrelse(int sig)
{
	sigset_t sigMask;
	sigemptyset(&sigMask);
	if (sigaddset(&sigMask, sig) == -1 || sigprocmask(SIG_UNBLOCK, &sigMask, NULL) == -1)
		return -1;
	return 0;
}

/* Sets disposition for sig to SIG_IGN
 * Returns 0 on success, or -1 on failure
 */
int sysv_sigignore(int sig)
{
	struct sigaction newsa;
	newsa.sa_handler = SIG_IGN;
	newsa.sa_flags = 0;
	sigemptyset(&newsa.sa_mask);
	if (sigaction(sig, &newsa, NULL) == -1)
		return -1;
	return 0;
}

/* 
 * Always returns -1 with errno set to EINTR
 */
int sysv_sigpause(int sig)
{
	sigset_t expectedSignals;
	sigprocmask(SIG_BLOCK, NULL, &expectedSignals);
	sigdelset(&expectedSignals, sig);
	sigsuspend(&expectedSignals);
}

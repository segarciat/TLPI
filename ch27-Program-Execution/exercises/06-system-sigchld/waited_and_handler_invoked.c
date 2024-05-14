#include <sys/wait.h> /* wait() */
#include <sys/types.h> /* pid_t */
#include <unistd.h> /* fork(), getpid() */
#include <stdlib.h> /* exit(), EXIT_FAILURE */
#include <stdio.h> /* fprintf(), stderr */
#include <string.h> /* strerror() */
#include <errno.h> /* errno */
#include <signal.h> /* sigset_t, sigemptyset(), sigaddset(), sigprocmask(), SIGCHLD */

static volatile sig_atomic_t gotSigchld = 0;

void
sigchldHandler(int sig)
{
	gotSigchld = 1;
}

int
main(int argc, char *argv[])
{
	pid_t pid;
	/* Block SIGCHLD */
	sigset_t blockMask, prevMask, pendingSignals;
	sigemptyset(&blockMask);
	sigaddset(&blockMask, SIGCHLD);
	if (sigprocmask(SIG_BLOCK, &blockMask, &prevMask) == -1) {
		fprintf(stderr, "Failed to add SIGCHLD to process signal mask\n");
		exit(EXIT_FAILURE);
	}
	sigpending(&pendingSignals);
	printf("Blocked SIGCHLD, which current %s in the set of pending signals\n",
		sigismember(&pendingSignals, SIGCHLD) ? "is" : "is not"
	);

	/* Install SIGCHLD handler */
	struct sigaction sa;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = sigchldHandler;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		fprintf(stderr, "Failed to install SIGCHLD handler\n");
		exit(EXIT_FAILURE);
	}
	printf("SIGCHLD handler installed in parent\n");

	switch(pid = fork()) {
		case  -1:
			fprintf(stderr, "Failed to fork\n");
			exit(EXIT_FAILURE);

		case  0:
			printf("Forked! Child (%ld) exiting...\n", (long) getpid());
			exit(EXIT_SUCCESS);
			break;
		default:
			printf("Parent waiting on child...\n");
			int status;
			if (wait(&status) == -1) {
				fprintf(stderr, "wait() failed: %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}
			printf("Parent has reaped child\n");

			sigpending(&pendingSignals);
			printf("SIGCHLD is still blocked, and it %s in the set of pending signals. Unblocking\n",
				sigismember(&pendingSignals, SIGCHLD) ? "is" : "is not"
			);
			if (sigprocmask(SIG_SETMASK, &prevMask, NULL) == -1) {
				fprintf(stderr, "Failed to restore process signal mask\n");
				exit(EXIT_FAILURE);
			}
			printf("SIGCHLD handler %s invoked\n", (gotSigchld) ? "was": "was not");

			exit(EXIT_SUCCESS);
	}
}

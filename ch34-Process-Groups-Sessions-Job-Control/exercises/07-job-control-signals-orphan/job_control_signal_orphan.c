#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

void
unix_error(char *msg)
{
	fprintf(stderr, "%s: %s\n", msg, strerror(errno));
	exit(EXIT_FAILURE);
}

void
handler(int sig)
{
	printf("Handler caught signal %d (%s)\n", sig, strsignal(sig));
}

void setSignalDisposition(int sig)
{
	struct sigaction sa;
	if (sigaction(sig, NULL, &sa) == -1)
		unix_error("Failed to get signal disposition");
	if (sa.sa_handler != SIG_IGN) {
		sa.sa_flags = 0;
		sigemptyset(&sa.sa_mask);
		sa.sa_handler = handler;
		if (sigaction(sig, &sa, NULL) == -1)
			unix_error("Failed to install signal handler");
	}
}

int
main(int argc, char *argv[])
{
	struct sigaction sa;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = handler;
	if (argc != 1) {
		setSignalDisposition(SIGTTOU);
		setSignalDisposition(SIGTTIN);
		setSignalDisposition(SIGTSTP);
	}

	switch(fork()) {
		case -1: unix_error("Failed to fork");
		case  0:
			printf("Child (%ld) will sleep\n", (long) getpid());
			sleep(2);
			printf("Child resumed, sending signals\n");
			raise(SIGTSTP);
			raise(SIGTTIN);
			raise(SIGTTOU);
			printf("Child exiting\n");
			exit(EXIT_SUCCESS);
		default:
			/* sleep(10); */
			printf("Parent exiting\n");
			_exit(EXIT_SUCCESS);
	}
}

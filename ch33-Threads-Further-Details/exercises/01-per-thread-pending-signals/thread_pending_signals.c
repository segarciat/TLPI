#include <pthread.h>
#include <signal.h> /* NSIG */
#define _GNU_SOURCE	/* Expose strsignal() in string.h */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

/* Display error message associated with errorNumber and exit */
void
unix_error(char *msg, int errorNumber)
{
	fprintf(stderr, "%s: %s\n", msg, strerror(errorNumber));
	exit(EXIT_FAILURE);
}

/* Block until a mutex is acquired, and then inspect and display set of pending signals */
void*
threadFunc(void *arg)
{
	int sig, cnt, s, threadIdentifier;
	sigset_t pendingSet;

	threadIdentifier = (long) arg;

	/* Acquire lock */
	s = pthread_mutex_lock(&mtx);
	if (s != 0)
		unix_error("Failed to lock mutex in peer thread", s);

	/* Get set of pending signals */
	if (sigpending(&pendingSet) != 0)
		unix_error("Failed to get set of pending signals in peer thread", errno);

	/* Display the name of the  pending signals */
	printf("Pending signals for thread: %d\n", threadIdentifier);
	for (sig = 1; sig < NSIG; sig++) {
		if (sigismember(&pendingSet, sig)) {
			cnt++;
			printf("\t%d (%s)\n", sig, strsignal(sig));
		}
	}
	if (cnt == 0) {
		printf("\t<empty signal set>\n");
	}

	/* Release lock */
	s = pthread_mutex_unlock(&mtx);
	if (s != 0)
		unix_error("Failed to unlock mutex in peer thread", s);

	return NULL;
}

int
main(int argc, char *argv[])
{
	int s;
	sigset_t blockSet;
	pthread_t tids[2];

	/* Block all signals */
	sigfillset(&blockSet);
	s = pthread_sigmask(SIG_BLOCK, &blockSet, NULL);
	if (s != 0)
		unix_error("Failed to set signal mask in main thread", s);

	/* Lock mutex to prevent threads from running until signals have been sent */
	s = pthread_mutex_lock(&mtx);
	if (s != 0)
		unix_error("Failed to lock mutex in main thread", s);

	/* Create peer threads */
	for (int i = 0; i < 2; i++) {
		s = pthread_create(&tids[i], NULL, threadFunc, (void *) i);
		if (s != 0) 
			unix_error("Failed to created thread", s);
	}

	/* Send signal to each thread */
	s = pthread_kill(tids[0], SIGUSR1);
	if (s != 0)
		unix_error("Failed to send SIGUSR1 signal to peer thread", s);
	s = pthread_kill(tids[1], SIGUSR2);
	if (s != 0)
		unix_error("Failed to send SIGUSR2 signal to peer thread", s);

	/* Unlock mutex to allow threads to inspect pending signals */
	s = pthread_mutex_unlock(&mtx);
	if (s != 0)
		unix_error("Failed to unlock mutex in main thread", s);

	/* Reap peer threads */
	for (int i = 0; i < 2; i++) {
		s = pthread_join(tids[i], NULL);
		if (s != 0)
			unix_error("Failed to join peer thread in main thread", s);
	}

	exit(EXIT_SUCCESS);
}

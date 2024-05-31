#include "one_time_init.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

static oticontrol_s someInitOnce = {
	.initRan = 0,
	.mtx = PTHREAD_MUTEX_INITIALIZER 
};

void
someInit()
{
	printf("Init ran!\n");
}

static void *
threadFunc(void *arg)
{
	printf("Running init, if it has not run\n");
	int s = one_time_init(&someInitOnce, someInit);
	if (s != 0) {
		fprintf(stderr, "Failed to run one time init: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	printf("exiting\n");
	return NULL;
}

int
main(int argc, char *argv[])
{
	pthread_t t1, t2;
	int s;
	
	/* Create two peer threads */
	s = pthread_create(&t1, NULL, threadFunc, NULL);
	if (s != 0) {
		fprintf(stderr, "Failed to create thread: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	s = pthread_create(&t2, NULL, threadFunc, NULL);
	if (s != 0) {
		fprintf(stderr, "Failed to create thread: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	/* Join threads */
	s = pthread_join(t1, NULL);
	if (s != 0) {
		fprintf(stderr, "Failed to join thread: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	s = pthread_join(t2, NULL);
	if (s != 0) {
		fprintf(stderr, "Failed to join thread: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}

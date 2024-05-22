#include <pthread.h> /* pthread_self(), pthread_join(), pthread_create(), pthread_t */
#include <string.h> /* strlen(), strerror() */
#include <stdio.h> /* printf(), fprintf(), stderr */
#include <stdlib.h> /* exit(), EXIT_FAILURE, EXIT_SUCCESS */
#include <unistd.h> /* sleep() */

static void *
threadFunc(void *arg)
{
	char *s = arg;
	printf("%s", s);
	int e = pthread_join(pthread_self(), NULL);
	if (e != 0) {
		fprintf(stderr, "Error in tread attempting to self join: %s\n", strerror(e));
		exit(EXIT_FAILURE);
	}

	return (void *) strlen(s);
}

int
main(int argc, char *argv[])
{
	pthread_t t1;
	void *res;
	int s;

	s = pthread_create(&t1, NULL, threadFunc, "Hello world\n");
	if (s != 0) {
		fprintf(stderr, "%s: Failed to create thread: %s\n", argv[0], strerror(s));
		exit(EXIT_FAILURE);
	}
	printf("Hello from main. Sleeping...\n");
	s = pthread_join(t1, &res);
	if (s != 0) {
		fprintf(stderr, "%s: Failed to join thread: %s\n", argv[0], strerror(s));
		exit(EXIT_FAILURE);
	}
	printf("Got %ld from thread! Exiting from main\n", (long) res);
	exit(EXIT_SUCCESS);
}

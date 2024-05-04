#include <stdio.h> /* printf() */
#include <signal.h> /* sig_atomic_t, struct sigaction, sigaction() */
#include <stdlib.h> /* exit(), EXIT_FAILURE, EXIT_SUCCESS */
#include <unistd.h> /* pause() */
#include "alarm.h" /* s_alarm() */
#include <time.h> /* clock_nanosleep() */
#include <errno.h> /* EINTR */

static volatile sig_atomic_t gotAlarm = 0;

void alarmHandler(int sig)
{
	gotAlarm = 1;
}

int
main(int argc, char *argv[])
{
	/* Establish disposition for SIGALRM */
	struct sigaction sa;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = alarmHandler;
	if (sigaction(SIGALRM, &sa, NULL) == -1)
		exit(EXIT_FAILURE);

	/* Setup alarm */
	unsigned int remaining = 5;
	printf("Setting alarm for %u seconds...\n", remaining);
	s_alarm(remaining);

	struct timespec sleepTime;
	sleepTime.tv_sec = 1;
	sleepTime.tv_nsec = 0;
	unsigned int counter = 0;

	/* Wait for alarm to expire */
	while (!gotAlarm) {
		int s = clock_nanosleep(CLOCK_REALTIME, 0, &sleepTime,NULL);
		if (s != 0 && s != EINTR) {
			fprintf(stderr, "%s: Error while sleeping\n", argv[0]);
			exit(EXIT_FAILURE);
		} else {
			printf("%u\n", ++counter);
		}
	}
	printf("Got alarm!\n");

	exit(EXIT_SUCCESS);
}

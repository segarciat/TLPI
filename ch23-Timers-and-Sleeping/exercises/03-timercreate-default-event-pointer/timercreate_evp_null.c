#define _POSIX_C_SOURCE 199309 /* Expose POSIX timer API with time.h */
#include <signal.h> /* struct siginfo_t, struct sigaction, sigaction(), SA_SIGINFO */
#include <time.h> /* timer_t, timer_create(), timer_t, CLOCK_REALTIME, struct itimerspec
                     timer_setime */
#include <stdlib.h> /* exit(), EXIT_FAILURE */
#include <stdio.h> /* printf(), fprintf(), stderr, NULL */
#include <unistd.h> /* pause() */

void alarmHandler(int sig, siginfo_t *siginfo, void *ucontext)
{
	/* UNSAFE: printf() is a non-async-signal-safe function */
	printf("si_signo=%d (%s), si_code=%d (%s), si_value=%d\n",
			siginfo->si_signo, "SIGALRM",
			siginfo->si_code,
			(siginfo->si_code == SI_TIMER) ? "SI_TIMER" : "other",
			siginfo->si_value.sival_int);
}

int
main(int argc, char *argv[])
{
	/* Establish disposition for SIGALRM */
	struct sigaction sa;
	sa.sa_flags = SA_SIGINFO;
	sigemptyset(&sa.sa_mask);
	sa.sa_sigaction = alarmHandler;
	if (sigaction(SIGALRM, &sa, NULL) == -1) {
		fprintf(stderr, "%s: Failed to set disposition for SIGALRM\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* Create POSIX timer */
	timer_t timerid;
	if (timer_create(CLOCK_REALTIME, NULL, &timerid) == -1) {
		fprintf(stderr, "%s: Failed to create timer\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	printf("Timer ID: %ld\n", (long) timerid);

	/* Arm timer */
	time_t duration = 3;
	printf("Setting timer for %ld seconds...\n", (long) duration);
	struct itimerspec its;
	its.it_value.tv_sec = 3;
	int timerFlags = 0;
	if (timer_settime(timerid, timerFlags, &its, NULL) == -1) {
		fprintf(stderr, "%s: Failed to arm timer\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* Wait for SIGALRM */
	pause();

	exit(EXIT_SUCCESS);
}

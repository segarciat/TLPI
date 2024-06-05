#include <sys/resource.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

#define _POSIX_C_SOURCE 200809L /* Expose sigwaitinfo() declaration in signal.h and POSIX clock APIs */
#include <signal.h>
#include <time.h>


#define CPU_SECONDS_CONSUMED 1
#define NANOS_PER_SEC 1000000000

void
unix_error(char *msg, int err)
{
	fprintf(stderr, "%s: %s\n", msg, strerror(err));
	exit(EXIT_FAILURE);
}

void
consumeCpu(size_t seconds)
{
	clockid_t cpuClockId;
	int s;
	struct timespec startTp, currentTp;

	s = clock_getcpuclockid(0, &cpuClockId);
	if (s != 0)
		unix_error("Failed to get cpu clock id", s);

	if (clock_gettime(cpuClockId, &startTp) == -1)
		unix_error("Failed to get starting CPU time", errno);
	
	for ( ; ; ) {
		if (clock_gettime(cpuClockId, &currentTp) == -1)
			unix_error("Failed to get CPU time elapsed", errno);
		double elapsed = currentTp.tv_sec - startTp.tv_sec;
		elapsed += (currentTp.tv_nsec - startTp.tv_nsec + 0.0) / NANOS_PER_SEC;
		if (elapsed > seconds)
			break;
	}
}

int
main(int argc, char *argv[])
{
	struct rusage resourceUsage;
	pid_t childPid, pid;

	/* Block SIGUSR1 which we will use as an IPC mechanism */
	sigset_t blockSet;
	sigemptyset(&blockSet);
	sigaddset(&blockSet, SIGUSR1);
	if (sigprocmask(SIG_BLOCK, &blockSet, NULL) == -1)
		unix_error("Failed to set signal mask", errno);

	switch(childPid = fork()) {
		case -1: unix_error("Fork failed to create children", errno);
		case  0: /* Child */
			pid = getpid();
			printf("Child [%ld]: Will consume CPU time\n", (long) pid);
			consumeCpu(CPU_SECONDS_CONSUMED);
			printf("Child [%ld]: Done consuming CPU time, signaling parent\n", (long) pid);
			if (kill(getppid(), SIGUSR1) == -1)
				unix_error("Failed to send SIGUSR1", errno);
			_exit(EXIT_SUCCESS);
		default: /* Parent */
			/* Wait for child to be done */
			if (sigwaitinfo(&blockSet, NULL) == -1)
				unix_error("sigwaitinfo failed while waiting for SIGUSR1", errno);

			/* Get child resource usage */
			if (getrusage(RUSAGE_CHILDREN, &resourceUsage) == -1)
				unix_error("Failed to get child reosurce usage", errno);
			pid = getpid();
			printf("Parent [%ld]: Child used %ld seconds and %ld nanoseconds. Reaping...\n",
					(long) pid,
					(long) resourceUsage.ru_utime.tv_sec + resourceUsage.ru_stime.tv_sec,
					(long) resourceUsage.ru_utime.tv_usec + resourceUsage.ru_stime.tv_usec
			);

			/* Reap child */
			if (waitpid(childPid, NULL, 0) == -1)
				unix_error("waitpid failed", errno);

			/* Get child resource usage again */
			if (getrusage(RUSAGE_CHILDREN, &resourceUsage) == -1)
				unix_error("Failed to get child reosurce usage", errno);
			printf("Parent [%ld]: Child used %ld seconds and %ld nanoseconds.\n",
					(long) pid,
					(long) resourceUsage.ru_utime.tv_sec + resourceUsage.ru_stime.tv_sec,
					(long) resourceUsage.ru_utime.tv_usec + resourceUsage.ru_stime.tv_usec
			);
			exit(EXIT_SUCCESS);
	}
}

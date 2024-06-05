#define _GNU_SOURCE /* Expose CPU affinity API in sched.h */
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#define _POSIX_C_SOURCE 200809L /* Expose clock_gettime() in time.h */
#include <time.h>

#define CPU_SECONDS_CONSUMED 3
#define NANOS_PER_SEC 1000000000L

void
unix_error(char *msg, int err)
{
	fprintf(stderr, "%s: %s\n", msg, strerror(err));
	exit(EXIT_FAILURE);
}

double
timeElapsed(struct timespec start, struct timespec end)
{
	long secs = end.tv_sec - start.tv_sec;
	long nanos = end.tv_nsec - start.tv_nsec;
	return secs + (nanos + 0.0) / (NANOS_PER_SEC);
}

void
consumeCPU(size_t seconds)
{
	struct timespec startTp, consumedTp, checkpointTp;
	clockid_t cpuClockId;
	pid_t pid;
	double elapsedSecs, totalElapsedSecs;

	pid = getpid();

	/* Get CPU clock ID */
	int s = clock_getcpuclockid(0, &cpuClockId); 
	if (s != 0)
		unix_error("Failed to get CPU clock ID", s);

	/* Get starting consumed CPU time */
	if (clock_gettime(cpuClockId, &startTp) == -1)
		unix_error("Failed to get starting CPU time", errno);
	checkpointTp = consumedTp = startTp;
	printf("Process [%ld]: Starting\n", (long) pid); 

	/* Consume CPU time */
	for ( ; ; ) {
		/* Get time consumed so far */
		if (clock_gettime(cpuClockId, &consumedTp) == -1)
			unix_error("Failed to get current CPU time", errno);

		/* Computed elapsed */
		elapsedSecs = timeElapsed(checkpointTp, consumedTp);
		if (elapsedSecs >= 0.25) {
			/* Display elapsied */
			printf("Process [%ld]: %.2f, yielding CPU\n", (long) pid, timeElapsed(startTp, consumedTp));
			checkpointTp = consumedTp;

			/* Yield CPU */
			if (sched_yield() == -1)
				unix_error("Failed to yield CPU", errno);
			if (consumedTp.tv_sec - startTp.tv_sec == seconds)
				break;
		}
	}
	printf("Process [%ld]: Done\n", (long) pid);
}

int
main(int argc, char *argv[])
{
	struct sched_param schedParam;
	int policy, priority;
	cpu_set_t cpuSet;
	
	/* Set scheduling policy; preserved after fork() */
	policy = SCHED_FIFO;
	if ((priority = sched_get_priority_max(policy)) == -1)
		unix_error("Failed to get min realtime policy priority", errno);
	schedParam.sched_priority = priority;
	if (sched_setscheduler(0, policy, &schedParam) == -1)
		unix_error("Failed to set realtime policy and priority", errno);
	
	/* Restrict to single CPU, moving there if not already there */
	CPU_ZERO(&cpuSet);
	CPU_SET(0, &cpuSet);
	if (sched_setaffinity(0, sizeof(cpu_set_t), &cpuSet) == -1)
		unix_error("Failed to set CPU affinity", errno);
	
	if (fork() == -1)
		unix_error("Failed to create child process", errno);

	/* Prevent from becoming a runaway process that locks up system */
	alarm(15);

	/* Both parent and child resume here, consuming CPU time */
	consumeCPU(CPU_SECONDS_CONSUMED);
	exit(EXIT_SUCCESS);
}

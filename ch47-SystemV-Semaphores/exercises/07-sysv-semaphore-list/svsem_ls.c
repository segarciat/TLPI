#define _GNU_SOURCE		/* Expose SEM_INFO and SEM_STAT */
#include <sys/types.h>	/* For portability */
#include <sys/sem.h>
#include <stdio.h> /* printf(), fprintf(), stderr */
#include <stdlib.h> /* exit(), EXIT_FAILURE, EXIT_SUCCESS */
#include <errno.h> /* errno */
#include <string.h> /* strerror() */

#if (_SEM_SEMUN_UNDEFINED == 1)
union semun {
	int val;
	struct semid_ds *buf;
	unsigned short *array;
#if defined(__linux__)
	struct seminfo *__buf;
#endif
};
#endif

void
unix_error(char *msg)
{
	fprintf(stderr, "%s: %s\n", msg, strerror(errno));
	exit(EXIT_FAILURE);
}

int
main(int arc, char *argv[])
{
	union semun arg;
	struct semid_ds ds;
	int maxSemIdx, idx, semid;
	arg.buf = &ds;

	/* Obtain all semaphores */
	maxSemIdx = semctl(/* Unused */ 0, /* Unused */ 0, SEM_INFO, arg);
	if (maxSemIdx == -1)
		unix_error("semctl() - Failed to SEM_INFO");

	/* Display header */
	printf("Maximum semaphore index: %d\n\n", maxSemIdx);
	printf("index    ID    nsems\n");

	/* Display information about each */
	for (idx = 0; idx <= maxSemIdx; idx++) {
		semid = semctl(idx, /* Unused */ 0, SEM_STAT, arg);
		if (semid == -1) {
			if (errno != EINVAL && errno != EACCES)
				fprintf(stderr, "semctl() - SEM_STAT: unexpected error");
			continue;
		}
		printf("%4d %8d %4ld\n", idx, semid, (long) arg.buf->sem_nsems);
	}
	exit(EXIT_SUCCESS);
}

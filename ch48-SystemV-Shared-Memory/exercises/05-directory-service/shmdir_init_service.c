#include "shmdir.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdarg.h>		/* va_list, va_start(), va_end() */
#include <stdio.h>		/* vfprintf(), fprintf(), stderr */
#include <stdlib.h>		/* exit(), EXIT_FAILURE */
#include <string.h>		/* strerror() */
#include <errno.h>		/* errno */
#include <unistd.h>		/* getpid() */

#if (_SEM_SEMUN_UNDEFINED == 1)	/* Check if glibc has defined union semun for us */
union semun {
	int val;
	struct semid_ds* buf;
	unsigned short *array;
#if defined(__linux__)
	struct seminfo *__buf;
#endif
};
#endif

/* Display user's message and the error message associated
   with the current valueof errno on stderr, then exit */
void
unix_error(char *fmt, ...)
{
	va_list varArgp;

	va_start(varArgp, fmt);
	if (fmt != NULL) {
		vfprintf(stderr, fmt, varArgp);
		fprintf(stderr, ": ");
	}
	fprintf(stderr, "%s\n", strerror(errno));
	va_end(varArgp);
	exit(EXIT_FAILURE);
}

/* Initializes and shared memory semaphore for directory service */

int
main(int arc,  char *argv[])
{
	int semid, shmid;
	union semun semarg;

	/* Create semaphore in set */
	semid = semget(SHMDIR_SEM_KEY, /* Single semaphore */ 1, IPC_CREAT | IPC_EXCL | OBJ_PERMS);
	if (semid == -1)
		unix_error("%s: semget() - Failed to create SysV semaphore", argv[0]);
	printf("%s: Created System V semaphore with id %d\n", argv[0], semid);

	/* Initialize semaphore to 1 (available) */
	semarg.val = 1;
	if (semctl(semid, /* Semaphore 0 in set */ 0, SETVAL, semarg) == -1) {
		fprintf(stderr, "%s: Failed to initiliaze semaphore 0 in set %d (%s). Deleting.\n",
			argv[0], semid, strerror(errno));
		if (semctl(semid, /* Ignored */ 0, IPC_RMID, /* Ignored */ semarg) == -1)
			unix_error("%s: semctl() - Failed to remove System V semaphore set with id %s", argv[0], semid);
		fprintf(stderr, "%s: Successfully removed semaphore set with id %d\n", argv[0], semid);
		exit(EXIT_FAILURE);
	}

	/* Create shared memory region */
	shmid = shmget(SHMDIR_SHM_KEY, SHMDIR_SHM_SEGSZ, IPC_CREAT | OBJ_PERMS);
	if (shmid == -1 && errno != EEXIST) {
		fprintf(stderr, "%s: shmget() - Failed to create SysV shared memory segment: %s.\n",
							argv[0], strerror(errno));
		fprintf(stderr, "%s: Deleting semaphore with id %d\n", argv[0], semid);
		if (semctl(semid, /* Ignored */ 0, IPC_RMID, /* Ignored */ semarg) == -1)
			unix_error("%s: semctl() - Failed to remove System V semaphore set with id %s", argv[0], semid);
		fprintf(stderr, "%s: Successfully removed semaphore set with id %d\n", argv[0], semid);
		exit(EXIT_FAILURE);
	}
	printf("%s: Created System V shared memory segment with id %d\n", argv[0], shmid);
	exit(EXIT_SUCCESS);
}

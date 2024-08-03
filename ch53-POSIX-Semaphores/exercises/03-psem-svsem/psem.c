#define _GNU_SOURCE				/* Expose semtimedop() */
#define _POSIX_C_SOURCE	199309L	/* Expose clock_gettime() from POSIX time API */
#include "psem.h"
#include <sys/types.h>			/* For portability. Also: key_t, struct timespec */
#include <sys/ipc.h>			/* For portability. Also: ftok() */
#include <sys/sem.h>			/* semget(), semctl() */
#include <stdlib.h>				/* malloc() */
#include <stddef.h>				/* NULL */
#include <stdio.h>				/* snprintf() */
#include <limits.h>				/* PATH_MAX */
#include <unistd.h>				/* close() */
#include <time.h>				/* clock_gettime(), CLOCK_REALTIME */
#include <errno.h>				/* errno */

#define FTOK_PROJ   1	/* Used to create System V IPCsemaphore keys */
#define NSEMS       1	/* We only allow operating on one semaphore in System V semaphore set */
#define SEMNUM      0	/* Since there is only one semaphore, it's always going to be semaphore 0 */
#define SEMWAIT   (-1)	/* Waiting for a semaphore means reduce its value by 1 */
#define SEMPOST     1	/* Posting to a semaphore means increasing its value by 1 */
#define NSEMOPS     1	/* We only ever do one operation at a time */

#if (_SEM_SEMUN_UNDEFINED == 1)
union semun {
    int val;
    struct semid_ds* buf;
    unsigned short* array;
#if defined(__linux__)
    struct seminfo* __buf;
#endif
};
#endif

psem_t* psem_open(const char *name, int oflag, mode_t mode, unsigned int value)
{
	/* Create a temporary file */
	char pathname[PATH_MAX];
	if (snprintf(pathname, PATH_MAX, "/tmp/%s", name) == -1)
		return PSEM_FAILED;
	int fd = open(pathname, oflag, mode);
	if (fd == -1)
		return PSEM_FAILED;


    /* Create System V IPC key */
    key_t key = ftok(pathname, FTOK_PROJ);
    if (key == -1) {
		close(fd);
        return PSEM_FAILED;
	}

    /* Set the right IPC flags */
    int flags = (oflag & O_CREAT) ? mode : 0;
    flags |= (oflag & O_CREAT) ? IPC_CREAT : 0;
    flags |= (oflag & O_EXCL)  ? IPC_EXCL  : 0;

    /* Get the System V semaphore ID */
    int semid = semget(key, NSEMS, flags);
    if (semid == -1) {
		close(fd);
        return PSEM_FAILED;
	}

    psem_t* psemp = malloc(sizeof(psem_t));
    if (psemp == NULL) {
		close(fd);
        return PSEM_FAILED;
	}
    psemp->semid = semid;
	psemp->fd = fd;

    if (oflag & O_CREAT) {
        /* Initialize sempahore if necessary */
        union semun arg;
        arg.val = value;
        if (semctl(semid, SEMNUM, SETVAL, arg) == -1) {
			close(fd);
			free(psemp);
            return PSEM_FAILED;
		}
    }
    return psemp;
}

int psem_close(psem_t *psem)
{
	return close(psem->fd);
}

int psem_unlink(const char *name)
{
	char pathname[PATH_MAX];
	if (snprintf(pathname, PATH_MAX, "/tmp/%s", name) == -1)
		return -1;
    /* Create System V IPC key */
    key_t key = ftok(pathname, FTOK_PROJ);
    if (key == -1)
        return -1;

    /* Get the System V semaphore ID */
    int semid = semget(key, NSEMS, O_RDWR);
	if (semid == -1)
		return -1;

	/* Delete the IPC object */
	union semun dummy;
	if (semctl(semid, SEMNUM, IPC_RMID, dummy) == -1)
		return -1;
	
	if (unlink(pathname) == -1)
		return -1;

    return 0;
}

static int _psem_wait(psem_t* psemp, int flags)
{
	struct sembuf sops;

	sops.sem_num = SEMNUM;
	sops.sem_op  = SEMWAIT;
	sops.sem_flg = flags;

	return semop(psemp->semid, &sops, NSEMOPS);
}

int psem_wait(psem_t* psemp)
{
	return _psem_wait(psemp, /* No flags */ 0);
}

int psem_trywait(psem_t* psemp)
{
	return _psem_wait(psemp, IPC_NOWAIT);
}

int psem_timedwait(psem_t* psemp, const struct timespec *abs_timeout)
{
	struct sembuf sops;
	sops.sem_num = SEMNUM;
	sops.sem_op = SEMWAIT;
	sops.sem_flg = 0;
	
	struct timespec currentTime;
	struct timespec timeout;
	if (clock_gettime(CLOCK_REALTIME, &currentTime) == -1)
		return -1;
	timeout.tv_sec = abs_timeout->tv_sec - currentTime.tv_sec;
	timeout.tv_nsec = abs_timeout->tv_nsec - currentTime.tv_nsec;
	if (timeout.tv_nsec < 0) {
		timeout.tv_sec -= 1;
		timeout.tv_nsec += 1000000000;
	}

	if (semtimedop(psemp->semid, &sops, NSEMOPS, &timeout) == -1) {
		if (errno == EAGAIN)
			errno = ETIMEDOUT;
		return -1;
	}
	return 0;
}

/* Increment semaphore value by 1 and returns. Returns 0 on success and -1 on error. */
int psem_post(psem_t* psem)
{
	struct sembuf sops;
	sops.sem_num = SEMNUM;
	sops.sem_op = SEMPOST;
	sops.sem_flg = 0;

	return semop(psem->semid, &sops, NSEMOPS);
}

/* Get the current value of the semaphore and return its value in the sval buffer.
  Returns 0 on success and -1 on error.
 */
int psem_getvalue(psem_t* psem, int *sval)
{
	union semun dummy;

	int result = semctl(psem->semid, SEMNUM, GETVAL, &dummy);
	if (result == -1)
		return -1;

	*sval = result;
	return 0;
}

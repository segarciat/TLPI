#include "event_flags.h"
#include <errno.h>

#define IS_SET 0
#define IS_CLEAR 1

int
setEventFlag(int semId, int semNum)
{
	union semun arg;
	arg.val = 0;

	return semctl(semId, semNum, SETVAL, arg);
}

int
clearEventFlag(int semId, int semNum)
{
	union semun arg;
	arg.val = 1;

	return semctl(semId, semNum, SETVAL, arg);
}

int
waitForEventFlag(int semId, int semNum)
{
	struct sembuf sops;

	sops.sem_num = semNum;
	sops.sem_op = 0;
	sops.sem_flg = 0;

	/* semop() may block and SA_RESTART setting does not cause semop to restart if interrupted */
	while(semop(semId, &sops, /* One operation */ 1) == -1)
		if (errno != EINTR)
			return -1;
	return 0;
}

int
getFlagState(int semId, int semNum, int *flagState)
{
	union semun arg;
	int semVal;

	if ((semVal = semctl(semId, semNum, GETVAL, arg)) == -1)
		return -1;
	*flagState = (semVal == 0) ? IS_SET : IS_CLEAR ;
	return 0;
}

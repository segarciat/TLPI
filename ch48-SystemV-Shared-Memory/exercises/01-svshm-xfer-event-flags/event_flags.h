#ifndef _SYSVSEM_EVENT_FLAGS_H	/* Prevent double inclusion */
#define _SYSVSEM_EVENT_FLAGS_H

#include <sys/types.h>	/* For portability */
#include <sys/sem.h>

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

/* An event flag is 'set' when the semaphore value is 0; it is 'clear' when
   the semaphore value is 1. */

/* Set event flag, returns 0 and success and -1 on failure */
int setEventFlag(int semId, int semNum);

/* Clear event flag, returns 0 and success and -1 on failure*/
int clearEventFlag(int semId, int semNum);

/* Block until flag is set, returns 0 and success and -1 on failure */
int waitForEventFlag(int semId, int semNum);

/* Obtain the current state of the flag, returned in flagState pointer
   returns 0 and success and -1 on failure*/
int getFlagState(int semId, int semNum, int *flagState);

#endif

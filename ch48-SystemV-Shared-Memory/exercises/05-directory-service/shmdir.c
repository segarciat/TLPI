#include "shmdir.h"
#include <sys/types.h>	/* For portability */
#include <sys/ipc.h>	/* For portability */
#include <sys/sem.h>	/* semget(), semop() */
#include <sys/shm.h>	/* shmget(), shmat(), shmdt() */
#include <string.h>		/* memcpy() */
#include <errno.h>
#include <stdbool.h>

/* Reserve the binary semaphore. Returns 0 on success, -1 on error */
static int
reserveSem(int semid)
{
	struct sembuf sops;

	sops.sem_num = 0;
	sops.sem_op = -1;
	sops.sem_flg = 0;

	/* May block and be interrupted */
	while (semop(semid, &sops, /* One operation */ 1) == -1)
		if (errno != EINTR)
			return -1;
	return 0;
}

/* Release the binary semaphore. Returns 0 on success, -1 on error */
static int
releaseSem(int semid)
{
	struct sembuf sops;

	sops.sem_num = 0;
	sops.sem_op = 1;
	sops.sem_flg = 0;

	return semop(semid, &sops, /* One operation */ 1);
}

/* Obtain structure to be passed to other methods to use directory. Returns 0 on success, -1 on error */
int
shmdir_init(struct shmdir_ds *ds)
{
	int semid, shmid;
	struct shmdir_ent *head;

	/* Obtain semaphore */
	semid = semget(SHMDIR_SEM_KEY, /* Existing semaphore, so 0 is ok */ 0, OBJ_PERMS);
	if (semid == -1)
		return -1;

	/* Obtain shared memory */
	shmid = shmget(SHMDIR_SHM_KEY, /* Existing segment, so 0 is ok */ 0, OBJ_PERMS);
	if (shmid == -1)
		return -1;
	
	/* Attach shared memory segment */
	head = shmat(shmid, /* Kernel decides address */ NULL, /* Read+Write */ 0);
	if (head == (void *) -1)
		return -1;

	ds->semid = semid;
	ds->shmbase = head;

	return 0;
}

/* Shuts down directory service. Returns 0 on success, -1 on error */
int
shmdir_shutdown(struct shmdir_ds *ds)
{
	if (shmdt(ds->shmbase) == -1)
		return -1;
	return 0;
}

/* 
 * newEnt is a non-null pointer with a name-value pair to publish
 * On error (no matching entry, directory is full, or duplicate key), sets errno, and returns -1
 */
int shmdir_add(const struct shmdir_ds *ds, const struct shmdir_ent *newEnt)
{
	struct shmdir_ent *ent;
	bool inserted;

	/* Name cannot be too long and must be nonempty */
	if (*(newEnt->nam) == '\0'
			|| strnlen(newEnt->nam, SHMDIR_NAM_MAX_LEN) == SHMDIR_NAM_MAX_LEN
			|| strnlen(newEnt->val, SHMDIR_VAL_MAX_LEN) == SHMDIR_VAL_MAX_LEN) {
		errno = EINVAL;
		return -1;
	}

	/* Reserve access to directory */
	if (reserveSem(ds->semid) == -1)
		return -1;
	
	/* First ensure there is no duplicate */
	for (ent = ds->shmbase; ent != ds->shmbase + SHMDIR_MAX_PAIRS; ent += sizeof(struct shmdir_ent)) {
		if (strncmp(ent->nam, newEnt->nam, SHMDIR_NAM_MAX_LEN) == 0) { /* Duplicate */
			releaseSem(ds->semid);
			errno = EINVAL;
			return -1;
		}
	}

	/* Find location to insert */
	inserted = false;
	for (ent = ds->shmbase; ent != ds->shmbase + SHMDIR_MAX_PAIRS; ent += sizeof(struct shmdir_ent)) {
		if (*(ent->nam) == '\0') {
			memcpy(ent, newEnt, sizeof(struct shmdir_ent));
			inserted = true;
			break;
		}
	}

	/* Relinquish directory access */
	if (releaseSem(ds->semid) == -1)
		return -1;

	/* No room to insert */
	if (!inserted) {
		errno = ENOMEM;
		return -1;
	}

	return 0;
}

/* Get value associated with a given name. Return 0 on success, -1 on error */
int shmdir_get(const struct shmdir_ds *ds, const char *name, struct shmdir_ent *usrEnt)
{
	struct shmdir_ent *ent;
	bool found;
	
	/* Disallow empty string */
	if (*name == '\0') {
		errno = EINVAL;
		return -1;
	}

	/* Reserve access to directory */
	if (reserveSem(ds->semid) == -1)
		return -1;

	/* Look for matching entry */
	found = false;
	for (ent = ds->shmbase; ent != ds->shmbase + SHMDIR_MAX_PAIRS; ent += sizeof(struct shmdir_ent)) {
		if (strncmp(name, ent->nam, SHMDIR_NAM_MAX_LEN - 1) == 0) {
			memcpy(usrEnt, ent, sizeof(struct shmdir_ent));
			found = true;
			break;
		}
	}

	/* Relinsquish directory access */
	if (releaseSem(ds->semid) == -1)
		return -1;

	/* No such entry */
	if (!found) {
		errno = EINVAL;
		return -1;
	}

	return 0;
}

/* Update value associated with a given name. Returns 0 on success, -1 on error */
int shmdir_upd(const struct shmdir_ds *ds, const struct shmdir_ent *usrEnt)
{
	struct shmdir_ent *ent;
	bool found;

	/* Disallow empty string */
	if (*(usrEnt->nam) == '\0') {
		errno = EINVAL;
		return -1;
	}

	/* Reserve access to directory */
	if (reserveSem(ds->semid) == -1)
		return -1;

	/* Look for matching entry */
	found = false;
	for (ent = ds->shmbase; ent != ds->shmbase + SHMDIR_MAX_PAIRS; ent += sizeof(struct shmdir_ent)) {
		if (strncmp(usrEnt->nam, ent->nam, SHMDIR_NAM_MAX_LEN - 1) == 0) {
			memcpy(ent->val, usrEnt->val, SHMDIR_VAL_MAX_LEN);
			found = true;
			break;
		}
	}

	/* Relinsquish directory access */
	if (releaseSem(ds->semid) == -1)
		return -1;

	/* No such entry */
	if (!found) {
		errno = EINVAL;
		return -1;
	}

	return 0;
}


/* Delete entry associated with a given name, returns 0 on success, and -1 on error */
int shmdir_del(const struct shmdir_ds *ds, const char *name)
{
	struct shmdir_ent *ent;
	bool found;

	/* Disallow empty string */
	if (*name == '\0') {
		errno = EINVAL;
		return -1;
	}

	/* Reserve access to directory */
	if (releaseSem(ds->semid) == -1)
		return -1;

	/* Look for matching entry */
	found = false;
	for (ent = ds->shmbase; ent != ds->shmbase + SHMDIR_MAX_PAIRS; ent += sizeof(struct shmdir_ent)) {
		if (strncmp(name, ent->nam, SHMDIR_NAM_MAX_LEN - 1) == 0) {
			memset(ent, 0, sizeof(struct shmdir_ent));
			found = true;
			break;
		}
	}

	/* Relinsquish directory access */
	if (releaseSem(ds->semid) == -1)
		return -1;

	/* No such entry */
	if (!found) {
		errno = EINVAL;
		return -1;
	}

	return 0;
}

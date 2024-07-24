#ifndef _SHM_DIRECTORY_SERVICE_H
#define _SHM_DIRECTORY_SERVICE_H
#include <sys/stat.h>	/* Permission flags */

#define SHMDIR_SHM_KEY 0x1234			/* Key for shared memory segment */
#define SHMDIR_SEM_KEY 0x5678			/* Key for semaphore set */

#define OBJ_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)

#define SHMDIR_NAM_MAX_LEN 128			/* Maximum bytes for a name, including null byte */
#define SHMDIR_VAL_MAX_LEN 128			/* Maximum bytes for a name, including null byte */

struct shmdir_ent {
	char nam[SHMDIR_NAM_MAX_LEN];
	char val[SHMDIR_VAL_MAX_LEN];
};

struct shmdir_ds {
	int semid;							/* ID of System V semaphore used by service */
	struct shmdir_ent *shmbase;			/* Base of System V shared memory segment used by service  */
};

#define SHMDIR_MAX_PAIRS 1024

#define SHMDIR_SHM_SEGSZ (SHMDIR_MAX_PAIRS * sizeof(struct shmdir_ent))

/* Sets up directory for usage in the client process and returns information needed for
   other service calls */
int shmdir_init(struct shmdir_ds *ds);

/* Shuts down the service */
int shmdir_shutdown(struct shmdir_ds *ds);

/* Create name-value pair. The ds buffer must have been obtained from shmdir_init() */
int shmdir_add(const struct shmdir_ds *ds, const struct shmdir_ent *newentry);

/* Get value associated with a given name. The ds buffer must have been obtained from shmdir_init() */
int shmdir_get(const struct shmdir_ds *ds, const char *name, struct shmdir_ent *ent);

/* Update value associated with a given name in ent->name.
   The ds buffer must have been obtained from shmdir_init() */
int shmdir_upd(const struct shmdir_ds *ds, const struct shmdir_ent *ent);

/* Delete key-value pair associated with given name. The ds buffer must have been obtained from shmdir_init() */
int shmdir_del(const struct shmdir_ds *ds, const char *name);

#endif

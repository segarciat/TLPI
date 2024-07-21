#define _GNU_SOURCE		/* Expose struct shminfo */
#include <sys/stat.h>	/* For portability */
#include <sys/ipc.h>	/* For portability */
#include <sys/shm.h>	/* shmctl() */
#include <stdio.h>		/* printf(), fprintf(), vfprintf(), stderr */
#include <stdlib.h>		/* exit(), EXIT_FAILURE, EXIT_SUCCESS */
#include <errno.h>		/* errno */
#include <string.h>		/* strerror() */
#include <stdarg.h>		/* va_list, va_start(), va_end() */
#include <pwd.h>		/* struct pw, getpwid() */

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

int
main(int argc, char *argv[])
{
	int maxShmIdx, shmid, idx;
	struct shminfo shminfobuf;
	struct shmid_ds ds;
	struct passwd *pw;
	
	/* Get largest shared memory segment index available */
	maxShmIdx = shmctl(/* Unused */ 0, SHM_INFO, (struct shmid_ds *) &shminfobuf);
	if (maxShmIdx == -1)
		unix_error("%s: shmctl() - SHM_INFO", argv[0]);

	/* Display information about each valid shared memory segment */
	printf("Maximum Index: %d\n", maxShmIdx);
	printf("index    id           owner      perms        size            nattach\n");
	for (idx = 0; idx <= maxShmIdx; idx++) {
		shmid = shmctl(idx, SHM_STAT, &ds);
		if (shmid == -1) {
			if (errno != EINVAL && errno != EACCES)
				fprintf(stderr, "%s: shmctl() failed to SHM_STAT with index %d: %s\n",
					argv[0], idx, strerror(errno));
			continue;
		}
		/* Get owner */
		if ((pw = getpwuid(ds.shm_perm.uid)) == NULL)
			unix_error("%s: Unable to retrieve owner of shared memory region", argv[0]);
		printf("%-8d %-12d %-10s %-12o %-15ld %-3ld\n",
			idx, (int) shmid, pw->pw_name, ds.shm_perm.mode, (long) ds.shm_segsz, (long) ds.shm_nattch);
	}
	exit(EXIT_SUCCESS);
}

#include <sys/types.h> /* For portability */
#include <sys/ipc.h> /* For portability */
#include <sys/shm.h>
#include <stdio.h> /* printf(), fprintf(), vfprintf(), stderr */
#include <stdlib.h> /* exit(), EXIT_FAILURE, EXIT_SUCCESS, strtol */
#include <errno.h> /* errno */
#include <string.h> /* strerror(), strcmp() */
#include <stdarg.h> /* va_list, va_start(), va_end() */
#include <time.h> /* ctime() */
#include <pwd.h> /* struct passwd, getpwid() */

#define USAGE_FORMAT "Usage: %s [--help] semid\n"

/* Print a user's message and the message associted with the current value of errno, then exit */
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
}

/* Parse string s as an integer, and returns its value in the buffer n.
 * On success returns 0, or -1 on error.
 */
long
parseInt(char *s, long *n)
{
	char *endp;

	errno = 0;
	*n = strtol(s, &endp, /* Any base */ 0);
	if (*endp != '\0')
	if (errno != 0 || *endp != '\0') {
		if (*endp != '\0')
			errno = EINVAL;
		return -1;
	}
	return 0;
}

int
main(int argc, char *argv[])
{
	long shmid;
	struct shmid_ds ds;
	struct passwd *pw;

	if (argc != 2) {
		fprintf(stderr, USAGE_FORMAT, argv[0]);
		exit(EXIT_FAILURE);
	}

	if (strcmp("--help", argv[1]) == 0) {
		printf(USAGE_FORMAT, argv[0]);
		exit(EXIT_SUCCESS);
	}

	/* Parse semaphore ID */
	if (parseInt(argv[1], &shmid) == -1)
		unix_error("%s: Invalid share memory ID: %s", argv[0], argv[1]);

	/* Get associated shared memory structure */
	if (shmctl(shmid, IPC_STAT, &ds) == -1)
		unix_error("%s: Unable to find associated shared memory structure", argv[0]);
	
	/* Get owner */
	if ((pw = getpwuid(ds.shm_perm.uid)) == NULL)
		unix_error("%s: Unable to retrieve owner of shared memory region", argv[0]);
	
	/* Display information about this segment */
	printf("\n");
	printf("Created by PID %ld at %s", (long) ds.shm_cpid, ctime(&ds.shm_ctime));
	printf("Last attached at %s", ctime(&ds.shm_atime));
	printf("Last dettached at %s", ctime(&ds.shm_dtime));
	printf("\n");
	printf("id           owner      perms        size            nattach\n");
	/* Display only lower 9 bits of permission mask */
	printf("%-12d %-10s %-12o %-15ld %-3ld\n",
			(int) shmid, pw->pw_name, 0x1FF & ds.shm_perm.mode, (long) ds.shm_segsz, (long) ds.shm_nattch);
	
	exit(EXIT_SUCCESS);
}

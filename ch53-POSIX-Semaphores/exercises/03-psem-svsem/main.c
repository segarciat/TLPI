#include "psem.h"
#define _POSIX_C_SOURCE 200809L	/* Expose clock_gettime() from POSIX clock API */
#include <stdio.h>				/* fprintf(), printf(), vfprintf() */
#include <stdlib.h>				/* exit(), EXIT_SUCCESS, EXIT_FAILURE */
#include <errno.h>				/* errno */
#include <string.h>				/* strerror() */
#include <errno.h>				/* errno */
#include <time.h>				/* clock_gettime(), CLOCK_REALTIME */
#include <stdarg.h>				/* va_list, va_start(), va_end() */
#include <stdbool.h>			/* errno, true, false */

/* Display a message on stderr, and optionally errno's corresponding message, then exit */
void
unix_error(bool showErrno, const char *fmt, ...)
{
	if (fmt != NULL) {
		va_list varArgp;
		va_start(varArgp, fmt);
		vfprintf(stderr, fmt, varArgp);
		va_end(varArgp);
	}
	if (showErrno)
		fprintf(stderr, "%s%s", (fmt != NULL) ? ": " : "", strerror(errno));
	fprintf(stderr, "\n");
	exit(EXIT_FAILURE);
}

int
main(int argc, char* argv[])
{
	const char *semName = "mysem";
	const int initialValue = 1;
	/* Open semaphore */
	psem_t* psemp = psem_open(semName, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, initialValue);
	if (psemp == PSEM_FAILED)
		unix_error(true, "%s: Failed to create semaphore", argv[0]);
	printf("Successfully opened semaphore initiaized (should be 1)\n");

	int sval;
	if (psem_getvalue(psemp, &sval) == -1)
		unix_error(true, "%s: Failed to get semaphore value", argv[0]);
	printf("Successfully retrieved semaphore value: %d (%s)\n", sval,
					(sval == initialValue) ? "as expected" : "mismatch");
	if (psem_post(psemp) == -1)
		unix_error("%s: Failed to post to semaphore", argv[0]);
	printf("Successfully did a semaphore post.\n");
	if (psem_getvalue(psemp, &sval) == -1)
		unix_error(true, "%s: Failed to get semaphore value after semaphore post", argv[0]);
	printf("Successfully retrieved semaphore value: %d (%s)\n", sval,
					(sval == initialValue + 1) ? "as expected" : "mismatch");

	const time_t timeoutSeconds = 3;
	struct timespec timeout;
	if (clock_gettime(CLOCK_REALTIME, &timeout) == -1)
		unix_error(true, "%s: Failed to get current time", argv[0]);
	timeout.tv_sec  += timeoutSeconds;

	if (psem_timedwait(psemp, &timeout) == -1) {
		if (errno == ETIMEDOUT)
			printf("%s: Timed out after %ld seconds, as expected\n", argv[0], (long) timeoutSeconds);
		else
			unix_error(true, "%s: psem_timedwait() failed to due", argv[0]);
	} else {
		printf("Timed wait returned immediately\n");
	}

	if (psem_close(psemp) == -1)
		unix_error(true, "%s: Failed to close semaphore", argv[0]);
	printf("Successfully closed semaphore.\n");
	if (psem_unlink(semName) == -1)
		unix_error(true, "%s: Failed to remove semaphore", argv[0]);
	printf("Successfully removed semaphore.\n");

	exit(EXIT_SUCCESS);
}

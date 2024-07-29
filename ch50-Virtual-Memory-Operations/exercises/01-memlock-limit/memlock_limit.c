#include <stdio.h>			/* fprintf(), vfprintf(), fprintf(), stderr */
#include <stdlib.h>			/* exit(), EXIT_FAILURE, EXIT_SUCCESS */
#include <errno.h>			/* errno */
#include <string.h>			/* strerror() */
#include <sys/resource.h>	/* getrlimit(), struct rlimit */
#include <unistd.h>			/* sysconf(), _SC_PAGESIZE */
#include <stdarg.h>			/* va_list, va_start(), va_end() */
#include <stdbool.h>		/* bool, true, false */
#include <sys/mman.h>		/* mmap() */

void
unix_error(bool showErrno, const char *fmt, ...)
{
	va_list varArgp;
	va_start(varArgp, fmt);

	if (fmt != NULL)
		vfprintf(stderr, fmt, varArgp);
	if (showErrno)
		fprintf(stderr, "%s%s", (fmt != NULL) ? ": " : "", strerror(errno));
	fprintf(stderr, "\n");
	va_end(varArgp);
	exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
	/* Create system page size, and verify that RLIMIT_MEMLOCK is large enough */
	errno = 0;
	long pageSize = sysconf(_SC_PAGESIZE);
	if (pageSize == -1)
		unix_error(errno != 0, "sysconf() - Failed to obtain value for _SC_PAGESIZE");
	printf("System page size: %ld\n", pageSize);
	struct rlimit memlockRLimit;
	if (getrlimit(RLIMIT_MEMLOCK, &memlockRLimit) == -1)
		unix_error(true, "gerlimit() - Failed to get resource limit for RLIMIT_MEMLOCK");
	printf("RLIMIT_MEMLOCK (current): %lld\n", (long long) memlockRLimit.rlim_cur);
	if (memlockRLimit.rlim_cur < 2 * pageSize) {
		fprintf(stderr, "RLIMIT_MEMLOCK is not large enough to lock two system pages\n");
		exit(EXIT_FAILURE);
	}
	/* Create a memory mapping */
	char *mmapAddr = mmap(NULL, 2 * pageSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (mmapAddr == MAP_FAILED)
		unix_error(true, "mmap() - Failed to create anonoymous mapping");

	/* Attempt to lock memory if this value is large enough */
	printf("Attempting to lock %ld bytes of virtual memory\n", 2 * pageSize);
	if (mlock(mmapAddr, 2 * pageSize) == -1)
		unix_error(true, "mlock() - Failed to lock requested memory region");
	
	/* Unlock memory */
	printf("Successfully locked requested memory region. Unlocking\n");
	if (munlock(mmapAddr, 2 * pageSize) == -1)
		unix_error(true, "munlock() - Failed to unlocker requested memory region");

	/* Lower the resource limit */
	printf("Lowering the hard limit of RLIMIT_MEMLOCK to one system page\n");
	memlockRLimit.rlim_cur = pageSize;
	memlockRLimit.rlim_max = pageSize;
	if (setrlimit(RLIMIT_MEMLOCK, &memlockRLimit) == -1)
		unix_error(true, "setrlimit() - Failed to lower the hard limit of RLIMIT_MEMLOCK");

	/* Attempt to lock memory again; should fail this time */
	printf("Attempting to lock two system pages of virtual memory\n");
	if (mlock(mmapAddr, 2 * pageSize) == -1) {
		fprintf(stderr, "mlock() - Failed to lock requested memory region: %s\n", strerror(errno));
		fprintf(stderr, "This error was %s\n", (errno == ENOMEM) ? "expected" : "unexpected");
		exit((errno == ENOMEM ) ? EXIT_SUCCESS : EXIT_FAILURE);
	}
	fprintf(stderr, "Expected mlock() should have failed, but it succeeded\n");
	exit(EXIT_FAILURE);
}

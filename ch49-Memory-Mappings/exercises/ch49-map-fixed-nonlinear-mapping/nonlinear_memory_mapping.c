#include <unistd.h>		/* sysconf(), _SC_PAGESIZE, unlink(), write() */
#include <errno.h>		/* errno */
#include <string.h>		/* strerror() */
#include <stdio.h>		/* fprintf(), vfprintf(), printf(), stderr */
#include <stdlib.h>		/* exit(), EXIT_FAILURE, EXIT_SUCCESS, mkstmp() */
#include <stdbool.h>	/* bool, false, true */
#include <stdarg.h>		/* va_list, va_start(), va_end() */
#include <sys/mman.h>	/* mmap() */

void
unix_error(bool showErrno, char *fmt, ...)
{
	va_list varArgp;
	va_start(varArgp, fmt);
	if (fmt != NULL)
		vfprintf(stderr, fmt, varArgp);
	if (showErrno)
		fprintf(stderr, "%s%s", fmt != NULL ? ": " : "", strerror(errno));
	fprintf(stderr, "\n");
	va_end(varArgp);
	exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
	/* Get the system page size */
	errno = 0;
	long pageSize = sysconf(_SC_PAGESIZE);
	if (pageSize == -1)
		unix_error(errno != 0, "%s: sysconf() - Failed to determine page size", argv[0]);
	printf("%s: Page size: %ld\n", argv[0], pageSize);

	/* Create an anonymous memory mapping that is three pages long */
	char *anonMapAddr = mmap(NULL, 3 * pageSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (anonMapAddr == MAP_FAILED)
		unix_error(true, "%s: mmap() - Failed to create anonoymous mapping", argv[0]);
	printf("%s: Before overlapping file mapping of temporary files over anonymous mapping\n", argv[0]);
	printf("%s: First byte in first page (region to be overlapped): %c\n", argv[0], *anonMapAddr);
	printf("%s: First byte in third page (region to be overlapped): %c\n", argv[0], anonMapAddr[2 * pageSize]);

	/* Create temporary file for overlay mapping */
	char fileTempA[] = "/tmp/A-map-fixed-overlay-XXXXXX";
	int fdA = mkstemp(fileTempA);
	if (fdA == -1)
		unix_error(true, "%s: mkstemp() - Failed to create temporary file %s", argv[0], fileTempA);
	if (unlink(fileTempA) == -1)
		unix_error(true, "%s: unlink() - Failed to unlink temporary file %s", argv[0], fileTempA);
	char c = 'A';
	if (write(fdA, &c, 1) == -1)
		unix_error(true, "%s: write() - Failed to write to temporary file %s", argv[0], fileTempA);

	char fileTempB[] = "/tmp/B-map-fixed-overlayXXXXXX";
	int fdB = mkstemp(fileTempB);
	if (fdB == -1)
		unix_error(true, "%s: mkstemp() - Failed to create temporary file %s", argv[0], fileTempB);
	if (unlink(fileTempB) == -1)
		unix_error(true, "%s: unlink() - Failed to unlink temporary file %s", argv[0], fileTempB);
	c = 'B';
	if (write(fdB, &c, 1) == -1)
		unix_error(true, "%s: write() - Failed to write to temporary file %s", argv[0], fileTempB);

	/* Overlay start of third page of mapping with a file mapping  */
	char *thirdPageOverlayMapAddr = mmap(anonMapAddr + 2 * pageSize, pageSize,
									PROT_READ | PROT_WRITE,
									MAP_PRIVATE | MAP_FIXED, fdA, 0);
	if (thirdPageOverlayMapAddr == MAP_FAILED)
		unix_error(true, "mmap() - Failed to create file map overlay at third page of anonyomus mapping");
	/* Overlay start of first page of mapping with a file mapping  */
	char *firstPageOverlayMapAddr = mmap(anonMapAddr, pageSize,
									PROT_READ | PROT_WRITE,
									MAP_PRIVATE | MAP_FIXED, fdB, 0);
	if (firstPageOverlayMapAddr == MAP_FAILED)
		unix_error(true, "mmap() - Failed to create file map overlay at first page of anonyomus mapping");
	printf("%s: After creating file mappings to overlap the temporary files over the anonymous mapping\n", argv[0]);
	printf("%s: First byte in first page (overlapped region): %c\n", argv[0], *anonMapAddr);
	printf("%s: First byte in third page (overlapped region): %c\n", argv[0], anonMapAddr[2 * pageSize]);
}

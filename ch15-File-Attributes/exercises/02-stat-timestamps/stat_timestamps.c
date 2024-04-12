#include <sys/stat.h>	/* stat */
#include <stdio.h>		/* fprintf */
#include <stdlib.h>		/* EXIT_SUCCESS, EXIT_FAILUER */

#define FILENAME "README.md"

int
main(int argc, char *argv[])
{
	struct stat statBuffer;
	if (stat(FILENAME, &statBuffer) == -1) {
		fprintf(stderr, "Failed to get metadata for %s\n", FILENAME);
		return EXIT_FAILURE;
	}
	printf("Timestamps for %s\n"
			"\tLast access: %ld\n"
			"\tLast modification: %ld\n"
			"\tLast status change: %ld\n",
			FILENAME, statBuffer.st_atime, statBuffer.st_mtime, statBuffer.st_ctime);
	return EXIT_SUCCESS;
}

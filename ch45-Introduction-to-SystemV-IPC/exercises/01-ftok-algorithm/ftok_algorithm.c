#include <sys/types.h> /* key_t */
#include <sys/ipc.h> /* ftok() */
#include <stdio.h> /* fprintf(), printf(), stderr, vsnprintf */
#include <stdlib.h> /* exit(), EXIT_FAILURE, mkstmp(), srand(), rand() */
#include <errno.h> /* errno */
#include <string.h> /* strerror() */
#include <unistd.h> /* unlink() */
#include <stdarg.h> /* va_list, va_start(), va_end() */
#include <stddef.h> /* NULL */
#include <time.h> /* time() */
#include <sys/stat.h> /* struct stat */

/* Display an error message an exit */
void
unix_error(char *fmt, ...)
{
#define USR_MSG_LEN 100
	char usrMsg[USR_MSG_LEN];
	va_list varArgList;

	va_start(varArgList, fmt);
	vsnprintf(usrMsg, USR_MSG_LEN, fmt, varArgList);
	va_end(varArgList);
	fprintf(stderr, "%s: %s\n", usrMsg, strerror(errno));
	exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
	key_t ipcKey;
	struct stat sb;
	int fd, proj;
	char tmpFile[] = "./tmpftokXXXXXX";

	/* Generate random proj */
	srand(time(NULL));
	proj = rand() % 0xFF + 1;
	printf("Generated proj (2 LSBs): %d\n", proj);

	/* Create temporary file */
	fd = mkstemp(tmpFile);
	if (fd == -1)
		unix_error("%s: Failed to make temporary file %s", argv[0], tmpFile);
	printf("Successfully created temporarily file with name: %s\n", tmpFile);

	if (fstat(fd, &sb) == -1)
		unix_error("%s: Failed to stat %s", argv[0], tmpFile);

	/* Use temporarily file to create key */
	ipcKey = ftok(tmpFile, proj);
	if (ipcKey == -1)
		unix_error("%s: Failed to create SysV IPC key", argv[0]);
	printf("Successfully created System V IPC key: %d\n\n", ipcKey);

	printf( "Filename           : %s\n"
			"Minor Device Number: %02lx\n"
			"i-node (2 LSBs)    : %04lx\n"
			"proj (LSB)         : %02x\n"
			"key (LSB)          : %08x\n",
				tmpFile, (long) sb.st_dev & 0xFF, (long) sb.st_ino & 0xFFFF, proj & 0xFF, (int) ipcKey);

	/* Delete the temporary file */
	printf("Deleting the temporary file\n");
	if (unlink(tmpFile) == -1)
		unix_error("%s: Failed to delete temporary file %s", argv[0], tmpFile);
	exit(EXIT_SUCCESS);
}

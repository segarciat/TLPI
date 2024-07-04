#include <unistd.h> /* pipe(), fork(), close(), read(), write() */
#include <stdio.h> /* fprintf(), printf(), vsnprintf(), stderr */
#include <stddef.h> /* NULL */
#include <stdlib.h> /* exit(), EXIT_FAILURE, EXIT_SUCCESS, strtol(), malloc(), free() */
#include <sys/wait.h> /* wait() */
#include <string.h> /* strerror(), memset() */
#include <errno.h> /* errno */
#include <stdarg.h> /* va_list, va_start(), va_end() */
#include <time.h> /* clock() */

/* Displays the error associated with the value of errno on stderr and exits */
void
unix_error(char *format, ...)
{
#define BUF_SIZE 512
	char buf[BUF_SIZE];

	va_list argList;
	va_start(argList, format);
	vsnprintf(buf, BUF_SIZE, format, argList);
	buf[BUF_SIZE-1] = '\0';
	fprintf(stderr, "%s: %s\n", buf, strerror(errno));
	va_end(argList);
	exit(EXIT_FAILURE);
}

/* Displays a usage string on stdout */
void
displayUsage(char *progName)
{
	printf("Usage: %s [-h | --help] number-of-blocks block-size\n\n"
			"Measures pipe bandwidth by forking a child that writes data blocks\n"
			"which are then read by the parent. Specify block size in bytes\n", progName);
}

/* Parses the integer given in string s and returns its value, or -1 on failure */
long
parseInt(char *s)
{
	char *endp;
	long n;

	errno = 0;
	n = strtol(s, &endp, 0); /* Any Base */
	if (errno != 0 || *endp != '\0' || n <= 0)
		return -1;
	return n;
}

int
main(int argc, char *argv[])
{
	long blockCount, blockSize;
	int pipefds[2];
	char *buf;
	ssize_t numRead;
	clock_t startTime, endTime;
	double elapsed;

	/* Display help information */
	if (argc > 1 && (strcmp("-h", argv[1]) == 0 || strcmp("--help", argv[1]) == 0)) {
		displayUsage(argv[0]);
		exit(EXIT_SUCCESS);
	}

	/* Enforce required arguments */
	if (argc != 3) {
		displayUsage(argv[0]);
		exit(EXIT_FAILURE);
	}

	/* Parse block count */
	if ((blockCount = parseInt(argv[1])) <= 0) {
		fprintf(stderr, "%s: Invalid block count: %s\n", argv[0], argv[1]);
		exit(EXIT_FAILURE);
	}

	/* Parse block size */
	if ((blockSize = parseInt(argv[2])) <= 0) {
		fprintf(stderr, "%s: Invalid block size: %s\n", argv[0], argv[2]);
		exit(EXIT_FAILURE);
	}

	/* Allocate buffer */
	if ((buf = malloc(blockSize)) == NULL)
		unix_error("%s: Failed to allocate buffer with %ld bytes", argv[0], blockSize);

	/* Create pipe */
	if (pipe(pipefds) == -1) {
		free(buf);
		unix_error("%s: Failed to create pipe", argv[0]);
	}
	memset(buf, 0, blockSize);

	/* Split into two processes */
	switch(fork()) {
		case -1:
			free(buf);
			unix_error("%s: Failed to fork child", argv[0]);
		case  0:	/* Child */
			/* Unused fd, and necessary to receive EPIPE when read end is closed */
			if (close(pipefds[0]) == -1) {
				free(buf);
				unix_error("%s: Failed to close read end of pipe in child", argv[0]);
			}
			/* Write as much data as possible */
			for (long i = 0; i < blockCount; i++) {
				if (write(pipefds[1], buf, blockSize) != blockSize) {
					free(buf);
					unix_error("%s: partial write or error to pipe from child", argv[0]);
				}
			}
			free(buf);

			/* Close the file descriptor */
			if (close(pipefds[1]) == -1)
				unix_error("%s: Failed to close write end of pipe in child" ,argv[0]);
			_exit(EXIT_SUCCESS);
		default:	/* Parent */
			/* Unused fd, and would block if remained open */
			if (close(pipefds[1]) == -1) {
				free(buf);
				unix_error(argv[0]);
			}

			/* Get start time */
			startTime = clock();
			if (startTime == (clock_t) -1) {
				free(buf);
				close(pipefds[0]);
				unix_error(argv[0]);
			}

			/* Read as much data as possible */
			for (long i = 0; i < blockCount; i++) {
				numRead = read(pipefds[0], buf, blockSize);
				if (numRead < 0) {
					free(buf);
					close(pipefds[0]);
					unix_error("%s: Failed to read from pipe in parent", argv[0]);
				}
			}
			/* Get end time */
			endTime = clock();
			if (endTime == (clock_t) -1) {
				free(buf);
				close(pipefds[0]);
				unix_error(argv[0]);
			}
			elapsed = (double) endTime / CLOCKS_PER_SEC - (double) startTime / CLOCKS_PER_SEC;
			free(buf);

			/* Close the file descriptor */
			if (close(pipefds[0]) == -1)
				unix_error("%s: Failed to close read end of pipe in parent\n", argv[0]);
			/* Reap child */
			wait(NULL);

			/* Display statistics */
			printf("Time elapsed: %.8f secs\nTotal Bytes Transferred: %ld\nBandwidth (bytes/sec): %.4f\n",
					elapsed, blockSize * blockCount, (blockSize * blockCount) / elapsed);
			exit(EXIT_SUCCESS);
	}
}

#include <sys/stat.h> /* mkfifo() */
#include <sys/types.h> /* ssize_t */
#include <fcntl.h> /* Permission constants, open(), open flags */
#include <unistd.h> /* read(), unlink() */
#include <errno.h> /* errno, EEXIST */
#include <stdarg.h> /* va_list, va_start(), va_end() */
#include <stdio.h> /* fprintf(), vsnprintf(), stderr */
#include <stdlib.h> /* exit(), EXIT_FAILURE, EXIT_SUCCESS */
#include <string.h> /* strerror() */
#include <linux/limits.h> /* PIPE_BUF */

#define FIFO_NAME "test_fifo_44_7"

static void
destroyFifo()
{
	unlink(FIFO_NAME);
}

static void
unix_error(char *fmt, ...)
{
#define MSG_MAX 100
	char usrMsg[MSG_MAX];
	va_list vargp;

	va_start(vargp, fmt);
	vsnprintf(usrMsg, MSG_MAX, fmt, vargp);
	va_end(vargp);
	fprintf(stderr, "ERROR: %s: %s\n", strerror(errno), usrMsg);
	exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
	int fifoFd;
	ssize_t bytesRead;
	char buf[PIPE_BUF];
	
	if (argc != 2 || (strcmp("r", argv[1]) != 0 && strcmp("w", argv[1]) != 0)) {
		fprintf(stderr, "Usage: %s <r|w>\n"
						"\t r to open FIFO for reading, w to open FIFO for writing\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if (atexit(destroyFifo) != 0)
		unix_error("Failed to add exit handler");

	/* Create a FIFO */
	if (mkfifo(FIFO_NAME, S_IRUSR | S_IWUSR | S_IWGRP) == -1
		&& errno != EEXIST) {
		unix_error("Failed to create FIFO %s", FIFO_NAME);
	}

	if (*argv[1] == 'w') {
		printf("Opening FIFO for writing...\n");
		fifoFd = open(FIFO_NAME, O_WRONLY | O_NONBLOCK);
		if (fifoFd == -1 && errno != ENXIO)
			unix_error("failed to open %s", FIFO_NAME);
		if (fifoFd != -1) {
			fprintf(stderr, "%s: open() succeeded, but it was expected to fail\n", argv[0]);
			exit(EXIT_FAILURE);
		}
		printf("%s: open() failed with ENXIO (%s), as expected\n", argv[0], strerror(errno));
		exit(EXIT_SUCCESS);
	}

	/* Open for reading */
	printf("Opening FIFO for reading...\n");
	fifoFd = open(FIFO_NAME, O_RDONLY | O_NONBLOCK);
	if (fifoFd == -1)
		unix_error("Failed to open FIFO %s for reading in nonblocking mode", FIFO_NAME);
	printf("Successfully opened FIFO for reading. Attempting to read while there is no writer...\n");
	bytesRead = read(fifoFd, buf, PIPE_BUF);
	if (bytesRead == -1)
		unix_error("failed to read from FIFO");
	if (bytesRead > 0) {
		fprintf(stderr, "%s: Expected to read 0 bytes from FIFO, but read %ld bytes\n", argv[0], (long) bytesRead);
		exit(EXIT_FAILURE);
	}
	printf("Detected EOF on FIFO file descriptor, as expected\n");
	exit(EXIT_SUCCESS);
}

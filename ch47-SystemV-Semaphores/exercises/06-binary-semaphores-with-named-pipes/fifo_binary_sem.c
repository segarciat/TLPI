#include "fifo_binary_sem.h"
#include <sys/types.h> /* ssize_t */
#include <sys/stat.h> /* mkfifo(), S_IRUSR, S_IWUSR, S_IRGRP, S_IWGRP */
#include <fcntl.h> /* open(), O_RDONLY, O_WRONLY, O_NONBLOCK */
#include <unistd.h> /* read(), write(), close() */
#include <errno.h> /* errno, EINTR */

#define ANY_BYTE 'x'

int
initSemAvailable(char *pathname, struct sempipe* sp_buf)
{
	int readFd, writeFd, savedErrno;
	ssize_t bytesWritten;

	/* Open two descriptors, for read and write */
	readFd = open(pathname, O_RDONLY | O_NONBLOCK);
	if (readFd == -1)
		return -1;
	writeFd = open(pathname, O_WRONLY);
	if (writeFd == -1) {
		savedErrno = errno;
		close(readFd);
		errno = savedErrno;
		return -1;
	}

	/* Write a single byte */
	char c;
	while ((bytesWritten = write(writeFd, &c, 1)) == -1)
		if (errno != EINTR)
			break;
	if (bytesWritten != 1) {
		savedErrno = errno;
		close(readFd);
		close(writeFd);
		unlink(pathname);
		errno = savedErrno;
		return -1;
	}
	sp_buf->readFd = readFd;
	sp_buf->writeFd = writeFd;
	return 0;
}

int
closeSem(struct sempipe* sem_fds_buf)
{
	if (close(sem_fds_buf->readFd) == -1 || close(sem_fds_buf->writeFd) == -1)
		return -1;
	return 0;
}

static int
_reserveSem(char *pathname, int flags)
{
	int fd, savedErrno;
	ssize_t bytesRead;
	char c;

	/* Open named pipe */
	fd = open(pathname, O_RDONLY | flags);
	if (fd == -1)
		return -1;
	
	/* Read one byte */
	while ((bytesRead = read(fd, &c, 1)) == -1)
		if (errno != EINTR)
			break;
	if (bytesRead != 1) {
		savedErrno = errno;
		close(fd);
		errno = savedErrno;
		return -1;
	}
	if (close(fd) == -1)
		return -1;
	return 0;
}

int
reserveSem(char *pathname)
{
	return _reserveSem(pathname, 0);
}

int
reserveSemNB(char *pathname)
{
	return _reserveSem(pathname, O_NONBLOCK);
}

int releaseSem(char *pathname)
{
	int fd, savedErrno;
	ssize_t bytesWritten;
	char c;

	/* Open the named pipe */
	fd = open(pathname, O_WRONLY);
	if (fd == -1)
		return -1;
	
	/* Write a single byte to named pipe, restarting if interrupted */
	c = ANY_BYTE;
	while ((bytesWritten = write(fd, &c, 1)) == -1)
		if (errno != EINTR)
			break;
	if (bytesWritten != 1) {
		savedErrno = errno;
		close(fd);
		errno = savedErrno;
		return -1;
	}

	/* Close named pipe */
	if (close(fd) == -1)
		return -1;
	return 0;
}

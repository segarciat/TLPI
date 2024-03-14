#include <fcntl.h> /* fcntl system call */
#include <errno.h> /* global errno variable and other constants, such as EBADF */
#include <unistd.h> /* close system call */

int dup(int oldfd)
{
	/* Verify the fd is valid */
	if (fcntl(oldfd, F_GETFL) == -1) {
		errno = EBADF;
		return -1;
	}
	int newfd = fcntl(oldfd, F_DUPFD, 0);
	if (newfd == -1) {
		if (errno == EINVAL)
			errno = EBADF;
		else
			errno = EMFILE;
		return -1;
	}
	return newfd;
}

int dup2(int oldfd, int newfd)
{
	/* Verify the fd is valid */
	if (fcntl(oldfd, F_GETFL) == -1) {
		errno = EBADF;
		return -1;
	}
	/* Do nothing if the fds are identical */
	if (oldfd == newfd)
		return newfd;
	/* Close the file at the requested fd */
	if (close(newfd) == -1) {
		return -1;
	}
	newfd = fcntl(oldfd, F_DUPFD, newfd);
	if (newfd == -1) {
		if (errno == EINVAL)
			errno = EBADF;
		return -1;
	}
	return newfd;
}

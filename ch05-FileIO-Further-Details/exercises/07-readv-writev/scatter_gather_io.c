#include <errno.h>  /* errno */
#include <stdlib.h> /* malloc, free */
#include "scatter_gather_io.h" /* struct iovec */

#define IOV_MAX 1024

static
ssize_t validateiovec(const struct iovec *iov, int iovcnt)
{
	/* Ensure iovcnt is non-negative, but also not too large */
	if (iovcnt < 0 || iovcnt > IOV_MAX) {
		errno = EINVAL;
		return -1;
	}
	/* Ensure the sum of the buffers does not overflow ssize_t */
	ssize_t totalRequested = 0;
	for (int i = 0; i < iovcnt; i++) {
		/* Invalid buffer length or request size overflowed */
		if (iov[i].iov_len <= 0 || (totalRequested += iov[i].iov_len) < iov[i].iov_len) {
			errno = EINVAL;
			return -1;
		}
	}
	return totalRequested;
}

ssize_t readv(int fd, const struct iovec *iov, int iovcnt)
{
	ssize_t totalRequested = validateiovec(iov, iovcnt);
	if (totalRequested == -1)
		return -1;
	if (totalRequested == 0)
		return 0;
	/* Allocate temporary buffer */
	char* buf = malloc((size_t) totalRequested);

	/* Check to system is not out of memory */
	if (buf == NULL)
		return -1;

	/* Request all data at once */
	ssize_t totalRead = read(fd, buf, totalRequested);
	if (totalRead == -1)
		return -1;

	/* Coopy data from dynamically allocated buffer to user's buffers */
	ssize_t copied = 0;
	for (int i = 0; i < iovcnt && copied < totalRead; i++) {
		char *v = iov[i].iov_base;
		for (size_t k = 0; k < iov[i].iov_len; k++)
			v[k] = buf[copied++];
	}
	free(buf);
	return totalRead;
}

ssize_t writev(int fd, const struct iovec *iov, int iovcnt)
{
	ssize_t totalRequested = validateiovec(iov, iovcnt);
	if (totalRequested == -1)
		return -1;
	if (totalRequested == 0)
		return 0;
	
	/* Allocate one large buffer */
	char* buf = malloc((size_t) totalRequested);
	if (buf == NULL)
		return -1;

	/* Concatenate data from io vector */
	char* p = buf;
	for (int i = 0; i < iovcnt && p < buf + totalRequested; i++) {
		char *v = iov[i].iov_base;
		for (size_t k = 0; k < iov[i].iov_len; k++)
			*p++ = v[k];
	}
	/* Write all data */
	ssize_t numWritten = write(fd, buf, totalRequested);
	if (numWritten == -1)
		return -1;
	free(buf);
	return numWritten;
}

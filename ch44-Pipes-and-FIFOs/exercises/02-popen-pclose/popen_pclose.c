#include "popen_pclose.h"
#include <stdio.h> /* fdopen(), fclose(), fileno() */
#include <stddef.h> /* NULL */
#include <sys/types.h> /* pid_t, size_t */
#include <unistd.h> /* fork(), execl(), pipe(), sysconf() */
#include <string.h> /* strcmp() */
#include <errno.h> /* errno, EINTR */
#include <sys/wait.h> /* waitpid() */
#include <stdlib.h> /* calloc(), malloc(), free() */

#define DEFAULT_MAX_OPEN_FDS 1024

struct _popen_s {
	pid_t cpid;
	FILE *fp;
};

static struct _popen_s **_popen_buf = NULL;

static struct _popen_s**
initBuffer()
{
	if (_popen_buf == NULL) {
		long maxOpenFiles = sysconf(_SC_OPEN_MAX);
		if (maxOpenFiles == -1)
			maxOpenFiles = DEFAULT_MAX_OPEN_FDS;
		_popen_buf = calloc((size_t) maxOpenFiles, sizeof(struct _popen_s*));
		if (_popen_buf == NULL)
			return NULL;
	}
	return _popen_buf;
}

FILE*
_popen(const char *command, const char *mode)
{
	pid_t childPid;
	int pfd[2], parentFd, childFd;
	FILE *fp;
	struct _popen_s *pipe_child_data;

	/* Initialize buffer */
	if (initBuffer() == NULL)
		return NULL;
	if ((pipe_child_data = malloc(sizeof(struct _popen_s))) == NULL)
		return NULL;

	if (pipe(pfd) == -1) {
		free(pipe_child_data);
		return NULL;
	}
	
	/* Only "r" or "w" are valid */
	if (strcmp("r", mode) == 0) {
		parentFd = pfd[0];
		childFd = pfd[1];
	} else if (strcmp("w", mode) == 0) {
		childFd = pfd[0];
		parentFd = pfd[1];
	} else {
		free(pipe_child_data);
		errno = EINVAL;
		return NULL;
	}

	switch(childPid = fork()) {
		case -1:
			break;
		case 0:		/* Child */
			/* Close fd unused by child */
			if (close(parentFd) == -1)
				_exit(127);

			int targetFd = *mode == 'r' ? STDOUT_FILENO : STDIN_FILENO;
			if (childFd != targetFd)
				if (dup2(childFd, targetFd) == -1 || close(childFd) == -1)
					_exit(127);

			execl("/bin/sh", "sh", "-c", command, (char *) NULL);
			_exit(127);
		default:	/* Parent falls through */
			break;
	}
	/* Parent continues here */
	if (childPid == -1 || close(childFd) == -1 || (fp = fdopen(parentFd, mode)) == NULL) {
		free(pipe_child_data);
		return NULL;
	}
	pipe_child_data->cpid = childPid;
	pipe_child_data->fp = fp;
	_popen_buf[parentFd] = pipe_child_data;
	return fp;
}

int
_pclose(FILE *stream)
{
	struct _popen_s *popen_data;
	int fd;
	int status;

	/* Get associated stream and child process id */
	if ((fd = fileno(stream)) == -1)
		return -1;
	popen_data = _popen_buf[fd];
	/* Close so that child's end of the pipe can also be closed properly */
	if (fclose(popen_data->fp) != 0)
		status = -1;
	/* Wait for child shell */
	while (waitpid(popen_data->cpid, &status, 0) == -1) {
		if (errno != EINTR) {
			status = -1;
			break;
		}
	}
	/* Close FILE stream */
	free(popen_data);
	return status;
}

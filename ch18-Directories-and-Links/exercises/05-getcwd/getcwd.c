#include <stddef.h> /* size_t */
#include <errno.h>	/* errno, EFAULT */
#include <sys/stat.h> /* struct stat, fstat() */
#include <fcntl.h>	/* open(), O_RDONLY */
#include <dirent.h> /* DIR, opendir() */
#include <unistd.h> /* chdir(), fchdir() */
#include <string.h> /* strcmp(), strncpy() */

/*
 * Places a null-terminated string containing the absolute pathname of the current
 * working directory into the allocated buffer pointed to by cwdbuf. The caller must
 * allocate cwdbuf to be at least size bytes in length.

 * On success, returns a pointer to cwdbuf as its result. If the pathname for the
 * current working directory exceeds size bytes, then it returns NULL and sets errno
 * to ERANGE.
 */
char *_getcwd(char *cwdbuf, size_t size)
{
	if (cwdbuf == NULL) {
		errno = EFAULT;
		return NULL;
	} else if (size == 0) {
		errno = EINVAL;
		return NULL;
	} else if (size < 1) { /* At least two characters to fit '/' and null terminator */
		errno = ERANGE;
		return NULL;
	}

	/* Save current working directory */
	struct stat oldcwdsb;
	int oldcwdfd = open(".", O_RDONLY);
	if (oldcwdfd == -1 || fstat(oldcwdfd, &oldcwdsb) == -1)
		return NULL;

	/* Build path string in cwdbuf from the end */
	size_t startOffset = size - 1;
	cwdbuf[startOffset] = '\0';

	/* Ascend through parent until root directory */
	for ( ; ; ) {
		/* Open parent directory to search */
		DIR *dirp;
		if (chdir("..") == -1 || (dirp = opendir(".")) == NULL)
			break;

		/* Find directory entry with matching i-node and device number */
		struct dirent *direntp;
		struct stat sb;
		for ( ; ; ) {
			errno = 0;
			direntp = readdir(dirp);
			if (direntp == NULL)
				break;
			if (direntp->d_ino != oldcwdsb.st_ino)
				continue;
			if (stat(direntp->d_name, &sb) == -1 || sb.st_dev == oldcwdsb.st_dev)
				break;
		}
		if (closedir(dirp) == -1 || errno != 0)
			break;
		/* Handle directory match by prepending path */
		if (direntp != NULL) {
			/* Current directory is root directory */
			if (strcmp(".", direntp->d_name) == 0) {
				if (cwdbuf[startOffset] == '\0')
					cwdbuf[--startOffset] = '/';
				break;
			}
			/* Non-root directory */
			size_t len = strlen(direntp->d_name);
			if (startOffset < (len + 1)) {
				errno = ERANGE;
				break;
			}
			startOffset -= (len + 1);
			cwdbuf[startOffset] = '/';
			strncpy(cwdbuf + startOffset + 1, direntp->d_name, len);
			/* In case path length exceeds PATH_MAX */
			if ((size - startOffset >= PATH_MAX) || stat(".", &oldcwdsb) == -1)
				break;
		}
	}
	/* Restore current working directory */
	int status = fchdir(oldcwdfd);
	if (close(oldcwdfd) == -1 || status == -1 || errno != 0)
		return NULL;
	if (startOffset != 0)
		strncpy(cwdbuf, cwdbuf + startOffset, (size - startOffset));
	return cwdbuf;
}

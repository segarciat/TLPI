#include "ftok.h"
#include <sys/stat.h> /* struct stat, stat() */
#include <errno.h> /* errno, EINVAL */

/* On success returns integer key, or -1 on error */
key_t _ftok(char *pathname, int proj)
{
	struct stat sb;

	if (proj == -1) {
		errno = EINVAL;
		return -1;
	}

	if (stat(pathname, &sb) == -1)
		return -1;
	return ((proj & 0xFF) << 24) | ((sb.st_dev & 0xFF) << 16) | (sb.st_ino & 0xFFFF);
}

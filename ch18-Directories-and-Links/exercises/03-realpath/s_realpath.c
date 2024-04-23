#include <limits.h> /* PATH_MAX */
#include <unistd.h> /* getcwd() */
#include <string.h> /* strlen(), strncmp() */
#include <sys/stat.h> /* struct stat, stat() */
#include "s_realpath.h"


/* 
 * Dereferences all symbolic links in pathname (null-terminated string)
 * and resolved all references to /. and /.. to produce a null-terminated
 * string containing the corresponding absolute pathname.
 * The resulting string is placed in the buffer pointed to by resolved_path,
 * which should be a character array of at least PATH_MAX bytes.
 * 
 * Returns pointer to resolved pathname on success, or NULL on error
 */
char*
_realpath(const char *pathname, char *resolved_path)
{
	if (pathname == NULL || resolved_path == NULL)
		return NULL;

	size_t len = 0;
	const char *p = pathname;
	/* Determine if it should be relative or absolute path */
	if (*pathname != '/') {
		if (getcwd(resolved_path, PATH_MAX) == NULL)
			return NULL;
		len = strlen(resolved_path);
	} else {
		resolved_path[len++] = '/';
		resolved_path[len] = '\0';
	}

	while (*p != '\0') {
		/* Skip all leading '/' */
		while (*p == '/')
			p++;

		/* Find next path separator */
		char *q = index(p, '/');
		size_t segmentLen = (q == NULL) ? strlen(p) : (q - p);

		if (segmentLen > 1 && strncmp("..", p, segmentLen) == 0) {
			/* Remove basename, if any */
			q = rindex(resolved_path, '/');
			len = (q - resolved_path);
			if (len == 0)
				resolved_path[len++] = '/';
			resolved_path[len] = '\0';
		} else if (strncmp(".", p, segmentLen) != 0) {
			if (len + 2 + segmentLen >= PATH_MAX)	/* Extra 2 for '/' and null terminator */
				return NULL;
			/* Attach segment to full path */
			if (resolved_path[len-1] != '/')
				resolved_path[len++] = '/';
			strncpy(resolved_path + len, p, segmentLen);
			len += segmentLen;
			resolved_path[len] = '\0';

			struct stat sb;
			if (stat(resolved_path, &sb) == -1)	/* Not an actual file */
				return NULL;
		}
		p += segmentLen;
	}
	if (len > PATH_MAX)
		return NULL;
	return resolved_path;
}

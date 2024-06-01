#include "paths.h"
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <errno.h>

/*
 * Helper to basename() and dirname() functions
 * Removes all trailing slash characters.
 * 
 * If pathname has slash characters, returns a pointer to either
 * the last non-trailing slash, or the first character in pathname
 *
 * Otherwise returns NULL.
 */
static void
findNoneTrailingSlash(char *pathname, char **result)
{
	char *p = (pathname == NULL || *pathname == '\0')
			? NULL
			: strrchr(pathname, '/');	

	if (p != NULL) {
		if (*(p + 1) == '\0') {					/* Remove all trailing slashes */
			while (p != pathname && *p == '/')
				*p-- = '\0';
			while (p != pathname && *p != '/')	/* Find non-trailing slashes */
				p--;
			if (p == pathname && *p != '/')
				p = NULL;
		}
	}
	*result = p;
}

/* Implementation of dirname() used by both thread-safe and non-re-entrant versions */
static char *
_dirname(char *pathname, char *buf, size_t size)
{
	char *p;
	findNoneTrailingSlash(pathname, &p);
	if (p != NULL && *p == '/') {						/* Remove non-leading repeated slashes */
		while (p != pathname && *(p - 1) == '/')
			*p-- = '\0';
	}
	if (p == NULL || *p != '/') {				/* No slashes */
		buf[0] = '.';
		buf[1] = '\0';
	} else if (*p == '/' && p == pathname) {	/* Only a slash */
		buf[0] = '/';
		buf[1] = '\0';
	} else {									/* Copy dirname */
		*p = '\0';
		strncpy(buf, pathname, size - 1);
		buf[size - 1] = '\0';
	}
	return buf;
}

/* Implementation of basename() used by both thread-safe and non-re-entrant versions */
static char*
_basename(char *pathname, char *buf, size_t size)
{
	char *p;
	findNoneTrailingSlash(pathname, &p);
	if (pathname == NULL || *pathname == '\0') {
		buf[0] = '.';
		buf[1] = '\0';
	} else if (p == NULL) {						/* No slashes */
		return pathname;
	} else if (*p == '/' && p == pathname) {	/* Contains only a slash */
		buf[0] = '/';
		buf[1] = '\0';
	} else {									/* Copy basename */
		*p++ = '\0';
		strncpy(buf, p, size - 1);
		buf[size - 1] = '\0';
	}
	return buf;
}

/* Non-reentrant implementation of dirname() */
char *
dirname_nr(char *pathname)
{
	static char buf[PATH_MAX];
	return _dirname(pathname, buf, PATH_MAX);
}

/* Non-reentrant implementation of basename() */
char *
basename_nr(char *pathname)
{
	static char buf[PATH_MAX];
	return _basename(pathname, buf, PATH_MAX);
}



static pthread_once_t once = PTHREAD_ONCE_INIT;
static pthread_key_t pathsKey;

/* Used only internally */
struct _pathsbufs {
	char *dirnamebuf;
	char *basenamebuf;
};

void
destructor(void *buf)
{
	struct _pathsbufs *pbufs = buf;
	free(pbufs->dirnamebuf);
	free(pbufs->basenamebuf);
	free(pbufs);
	printf("Destructor exiting\n");
}

static void
createKey(void)
{
	int s;

	 /* Allocate unique thread-specific data key, and safe the address
	  of the destructor for thread-specific data buffers */
	printf("Created key (should only run once)\n\n");
	s = pthread_key_create(&pathsKey, destructor);
	if (s != 0) {
		fprintf(stderr, "Failed to create Pthread key: %s\n", strerror(s));
		exit(EXIT_FAILURE);
	}
}

static struct _pathsbufs*
_getPathBuf()
{
	int s;
	struct _pathsbufs *pbufs;

	/* First caller must allocate key for thread-specific data */
	s = pthread_once(&once, createKey);

	pbufs = pthread_getspecific(pathsKey);
	if (pbufs == NULL) {
		pbufs = malloc(sizeof(struct _pathsbufs));
		if (pbufs == NULL
			|| (pbufs->dirnamebuf = malloc(PATH_MAX)) == NULL
			|| (pbufs->basenamebuf = malloc(PATH_MAX)) == NULL) {
			fprintf(stderr, "Failed to allocate thread-specific data: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		s = pthread_setspecific(pathsKey, pbufs);
		if (s != 0) {
			fprintf(stderr, "Failed to set thread-specific data: %s\n", strerror(s));
			exit(EXIT_FAILURE);
		}
	}
	return pbufs;
}

char *
dirname_ts(char *pathname)
{
	struct _pathsbufs *pbufs = _getPathBuf();
	return _dirname(pathname, pbufs->dirnamebuf, PATH_MAX);
}

char *
basename_ts(char *pathname)
{
	struct _pathsbufs *pbufs = _getPathBuf();
	return _basename(pathname, pbufs->basenamebuf, PATH_MAX);
}

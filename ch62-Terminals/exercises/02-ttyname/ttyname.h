#ifndef _TTYNAME_H
#define _TTYNAME_H

#include <sys/types.h>  /* size_t */
#include <limits.h>     /* NAME_MAX */

/* NAME_MAX excludes null byte, but sizeof("/dev/pts") incldues it */
#define TTYNAME_MAX (sizeof("/dev/pts/") + NAME_MAX)

/* Returns the name of the associated terminal device in ttybuf, which should be at least
   of size TTYNAME_MAX. On success returns 0, and on failure, returns -1. */
int _ttyname(int fd, char *ttybuf, size_t bufsize);

#endif

#define _DEFAULT_SOURCE /* Exposes definition of putenv() in stdlib */

#include <stdio.h>  // snprintf
#include <stdlib.h> // getenv, putenv, malloc
#include <string.h> // strlen, strncmp
#include <stddef.h> // size_t
#include "set_unset_env.h"

extern char **environ;

int my_setenv(const char *name, const char *value, int overwrite)
{
	if (!name || !value)
		return -1;

	/* Variable exists and overwrite is 0 (no overwrite) */
	if (!overwrite && getenv(name))
		return 0;

	size_t nameLen = strlen(name);
	size_t valueLen = strlen(value);
	size_t totalLen = nameLen + valueLen;

	/* Assign 2 extra bytes: for `=` and null terminator character */
	if (totalLen < nameLen || (totalLen += 2) < nameLen)
		return -1; // size_t overflow
	
	char *buf = malloc(totalLen);
	if (buf == NULL)
		return -1;

    /* Build string name=value format */
    memcpy(buf, name, nameLen);
    buf[nameLen] = '=';
    strcpy(buf + nameLen + 1, value);

    if (!putenv(buf))
        return -1;
    return 0;
}

int my_unsetenv(const char *name)
{
	if (!name)
		return -1;
	if (!environ)
		return 0;

	for (char **ep = environ; *ep != NULL; ep++)
		if (strcmp(*ep, name) == 0 && !putenv(*ep))
			return -1; // non-standard glibc behavior
	return 0;
}

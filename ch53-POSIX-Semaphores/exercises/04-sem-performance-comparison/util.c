#include "util.h"
#include <stddef.h>     /* NULL */
#include <stdarg.h>     /* va_list, va_start(), va_end() */
#include <stdio.h>      /* fprintf(), vfprintf(), stderr */
#include <errno.h>      /* errno */
#include <string.h>     /* strerror() */
#include <stdlib.h>     /* exit(), EXIT_FAILURE, strtol */

void
errorExit(bool showErrno, const char* fmt, ...)
{
    if (fmt != NULL) {
        va_list varArgp;
        va_start(varArgp, fmt);
        vfprintf(stderr, fmt, varArgp);
        va_end(varArgp);
    }
    if (showErrno)
        fprintf(stderr, "%s%s", (fmt != NULL) ? ": " : "", strerror(errno));
    fprintf(stderr, "\n");
    exit(EXIT_FAILURE);
}

/* Parses a string as an integer, and places the result in the buffer n.
   Returns 0 if successful, or -1 if an error or invalid counter is encountered
*/
int
parseLong(const char *s, long* n)
{
    char* endp;
    long m;
    
    errno = 0;
    m = strtol(s, &endp, /* Any base */ 0);
    if (errno != 0 || *endp != '\0')
        return -1;
    *n = m;
    return 0;
}

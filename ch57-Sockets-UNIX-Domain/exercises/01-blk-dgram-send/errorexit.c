#include <stdio.h>      /* fprintf(), printf() */
#include <stdlib.h>     /* exit(), EXIT_FAILURE, EXIT_SUCCESS */
#include <string.h>     /* strerror() */
#include <errno.h>      /* errno */

void
errorExit(const char *msg)
{
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(EXIT_FAILURE);
}

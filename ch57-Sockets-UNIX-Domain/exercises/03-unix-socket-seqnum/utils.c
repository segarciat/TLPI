#include <stdio.h>  /* fprintf(), stderr */
#include <stdlib.h> /* exit(), EXIT_FAILURE, strtol() */
#include <string.h> /* strerror() */
#include <errno.h>  /* errno */


/* Display given message and the message associated with the current value of errno to stderr, and then exit. */
void
errorExit(const char *msg)
{
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(EXIT_FAILURE);
}

/* Parse string s as an integer and place result in buffer n, then return 0. On failure, return -1. */
int
parseLong(const char *s, long *n)
{
    long m;
    char *endp;

    errno = 0;
    m = strtol(s, &endp, 0);
    if (errno != 0 || *endp != '\0')
        return -1;

    if (n != NULL);
        *n = m;
    return 0;
}

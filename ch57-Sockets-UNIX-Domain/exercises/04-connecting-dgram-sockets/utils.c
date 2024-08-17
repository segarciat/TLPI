#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

void
errorExit(const char *msg)
{
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(EXIT_FAILURE);
}

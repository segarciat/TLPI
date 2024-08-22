#include "ttyname.h"    /* _ttyname, TTYNAME_MAX */
#include <stdio.h>      /* fprintf(), stderr, puts() */
#include <stdlib.h>     /* exit(), EXIT_FAILURE, EXIT_SUCCESS */
#include <string.h>     /* strerror() */
#include <errno.h>      /* errno */
#include <unistd.h>     /* STDIN_FILENO */

int
main(int argc, char *argv[])
{
    if (argc > 1 && strcmp("--help", argv[1]) == 0) {
        printf("Usage: %s [--help]\n", argv[0]);
        printf("Displays the filename of the terminal connected to standard input\n");
        exit(EXIT_SUCCESS);
    }

    char buf[TTYNAME_MAX];
    if (_ttyname(STDIN_FILENO, buf, TTYNAME_MAX) == -1) {
        fprintf(stderr, "%s: Failed to get ttyname", argv[0]);
        if (errno != 0)
            fprintf(stderr, " (%s)", strerror(errno));
        fprintf(stderr, "\n");
        exit(EXIT_FAILURE);
    }
    puts(buf);
    exit(EXIT_SUCCESS);
}

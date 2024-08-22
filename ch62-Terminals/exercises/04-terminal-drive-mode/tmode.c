#include <stdio.h>  /* fprintf(), printf(), stderr, L_ctermid */
#include <stddef.h> /* NULL */
#include <stdlib.h> /* exit(), EXIT_FAILURE, EXIT_SUCCESS*/
#include <string.h> /* strerror() */
#include <errno.h>  /* errno */
#include <fcntl.h>  /* open(), O_RDONLY */
#include <unistd.h> /* close() */

#include <termios.h>

static void
errorExit(const char *msg)
{
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
    char cttyname[L_ctermid];
    if(ctermid(cttyname) == NULL)
        errorExit("ctermid(): Failed to get controlling terminal name");

    int fd = open(cttyname, O_RDONLY);
    if (fd == -1)
        errorExit("open(): Failed to open controlling terminal");

    struct termios tios;
    if (tcgetattr(fd, &tios) == -1)
        errorExit("tcgetattr(): Failed to get terminal settings");
    if (tios.c_lflag & ICANON) {
        printf("Canonical\n");
    } else {
        printf("Noncanonical\nMIN=%ld\nTIME=%ld\n", (long) tios.c_cc[VMIN], (long) tios.c_cc[VTIME]);
    }
    if (close(fd) == -1)
        errorExit("close(): Failed to close controlling terminal file");
    exit(EXIT_SUCCESS);
}

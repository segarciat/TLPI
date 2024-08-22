#include "getpass.h"
#include <termios.h>    /* struct termios, tcgetattr(), tcsetattr(), etc */
#include <errno.h>      /* errno, EINVAL */
#include <stddef.h>     /* NULL */
#include <stdio.h>      /* ctermid(), L_ctermid */
#include <fcntl.h>      /* open(), O_* constants */
#include <unistd.h>     /* close() */

char*
_getpass(const char *prompt, char *passbuf, size_t bufsize)
{
    static char pass[PASS_MAX];
    if (prompt == NULL || bufsize < PASS_MAX) {
        errno = EINVAL;
        return NULL;
    }
    
    /* Open controlling terminal */
    char cttyname[L_ctermid];
    if (ctermid(cttyname) == NULL)
        return NULL;

    int fd = open(cttyname, O_RDONLY);
    if (fd == -1)
        return NULL;

    /* Fetch all terminal settings and save them */
    struct termios originalTios;
    if (tcgetattr(fd, &originalTios) == -1) {
        close(fd);
        return NULL;
    }

    /* Disable special character processing, except EOF */
    struct termios tempTios = originalTios;
    tempTios.c_lflag &= ~(ECHO);
    int spTermDisableChar = fpathconf(fd, _PC_VDISABLE);
    if (spTermDisableChar == -1) {
        close(fd);
        return NULL;
    }
    for (cc_t sptchar = 0; sptchar < NCCS; sptchar++)
        if (sptchar != VEOF)
            tempTios.c_cc[sptchar] = spTermDisableChar;

    if (tcsetattr(fd, TCSAFLUSH, &tempTios) == -1) {
        close(fd);
        return NULL;
    }
    
    /* Show the prompt */
    printf("%s", prompt);
    fflush(stdout);

    /* Read input until EOF, newline, or no room left */
    char *p = passbuf;
    size_t totalRead = 0;
    ssize_t bytesRead;

    for(;;) {
        bytesRead = read(fd, p, PASS_MAX - totalRead);
        if (bytesRead == -1)
            break;  /* Error */
        totalRead += bytesRead;
        p += bytesRead;
        if (bytesRead == 0 || totalRead >= PASS_MAX || passbuf[totalRead - 1] == '\n')
            break;  /* EOF, newline, or out of room */
    }

    /* Restore terminal settings */
    int termRestoreStatus = tcsetattr(fd, TCSAFLUSH, &originalTios);
    if (close(fd) == -1 || bytesRead == -1 || termRestoreStatus == -1)
        return NULL;

    if (passbuf[totalRead - 1] == '\n') /* Strip newline character */
        passbuf[totalRead - 1] = '\0';
    return passbuf;
}

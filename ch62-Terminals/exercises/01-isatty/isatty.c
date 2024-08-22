#include "isatty.h"
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>

int
_isatty(int fd)
{
    struct termios tp;
    if (fcntl(fd, F_GETFD) == -1 && errno == EBADF) /* Check FD is valid */
        return 0;
    if (tcgetattr(fd, &tp) == -1) {                /* Valid but not a tty */
        errno = ENOTTY;
        return 0;
    }
    return 1;
}

#include "pipe_sp.h"
#include <sys/socket.h>     /* socketpair(), shutdown(), AF_UNIX, SOCK_STREAM */
#include <errno.h>          /* errno, EINVAL */
#include <stddef.h>         /* NULL */


int pipe_sp(int filedes[2])
{
    if (filedes == NULL) {
        errno = EINVAL;
        return -1;
    }
    /* Created connected byte stream sockets in the UNIX domain */
    int sockfds[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockfds) == -1)
        return -1;

    /* Allow on read on first fd */
    if (shutdown(sockfds[0], SHUT_WR) == -1)
        return -1;

    /* Allow on write on send fd */
    if (shutdown(sockfds[1], SHUT_RD) == -1)
        return -1;

    filedes[0] = sockfds[0];
    filedes[1] = sockfds[1];
    return 0;
}

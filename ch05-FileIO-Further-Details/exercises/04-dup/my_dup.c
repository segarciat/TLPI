#include <fcntl.h>  /* fcntl system call */
#include <errno.h>  /* global errno variable and other constants, such as EBADF */
#include <unistd.h> /* close system call */
#include "my_dup.h"

int my_dup(int oldfd)
{
    /* Verify the fd is valid */
    if (fcntl(oldfd, F_GETFL) == -1) {
        errno = EBADF;
        return -1;
    }
    int newfd = fcntl(oldfd, F_DUPFD, 0 /* lowest desired fd */);
    if (newfd == -1) {
        if (errno == EINVAL)
            errno = EBADF;
        else
            errno = EMFILE;
        return -1;
    }
    return newfd;
}

int my_dup2(int oldfd, int newfd)
{
    /* Verify the fd is valid */
    if (fcntl(oldfd, F_GETFL) == -1) {
        errno = EBADF;
        return -1;
    }
    /* Do nothing if the fds are identical */
    if (oldfd == newfd)
        return newfd;

    /* Unlike dup2, this closes newfd even if fcntl fails its DUPFD operation */
    close(newfd);

    newfd = fcntl(oldfd, F_DUPFD, newfd);
    if (newfd == -1) {
        if (errno == EINVAL)
            errno = EBADF;
        return -1;
    }
    return newfd;
}

#include "sendfile.h"
#include "rdwrn.h"      /* readn(), writen() */
#include <sys/stat.h>   /* struct stat, S_ISREG, S_ISSOCK */
#include <unistd.h>     /* fstat(), lseek(), SEEK_SET, SEEK_CUR */
#include <errno.h>      /* errno, EINVAL, EINTR */
#include <stddef.h>     /* NULL */

#define BUF_SIZE 4096



ssize_t sendfile(int out_fd, int in_fd, off_t *offset, size_t count)
{
    struct stat sb;

    /* Invalid count */
    if (count <= 0) {
        errno = EINVAL;
        return -1;
    }

    /* in_fd must be point to a regular file */
    if (fstat(in_fd, &sb) == -1)
        return -1;
    if (!S_ISREG(in_fd)) {
        errno = EINVAL;
        return -1;
    }

    /* out_fd must point to a socket */
    if (fstat(out_fd, &sb) == -1)
        return -1;
    if (!S_ISSOCK(out_fd)) {
        errno = EINVAL;
        return -1;
    }

    /* If necessary, save the old file position and set where we begin reading */
    ssize_t startPos = lseek(in_fd, 0, SEEK_CUR);
    if (startPos == -1)
        return -1;
    if (offset != NULL && lseek(in_fd, *offset, SEEK_SET) == -1)
        return -1;

    /* Transfer data chunks at a time */
    ssize_t totalRemaining = count;
    char buf[BUF_SIZE];
    for (;;) {
        size_t chunkSize = (totalRemaining > BUF_SIZE) ? BUF_SIZE : totalRemaining;
        ssize_t numRead = readn(in_fd, buf, chunkSize);
        if (numRead == -1) {
            lseek(in_fd, startPos, SEEK_SET);
            return -1;
        }
        if (numRead == 0)
            break;
        if (writen(out_fd, buf, numRead) == -1) {
            lseek(in_fd, startPos, SEEK_SET);
            return -1;
        }

        totalRemaining -= numRead;
        if (totalRemaining == 0)
            break;
    }

    ssize_t totalTransferred = count - totalRemaining;


    /* Return new file position and leave actual file offset unchanged */
    if (offset != NULL) {
        *offset += totalTransferred;
        if (lseek(in_fd, startPos, SEEK_SET) == -1)
            return -1;
    }

    return totalTransferred;
}

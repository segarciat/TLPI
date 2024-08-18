#include "rlbuf.h"
#include <errno.h>  /* errno, EINVAL, EINTR */
#include <unistd.h> /* read(), write() */
#include <stddef.h> /* NULL */

int
readLineBufInit(int fd, struct rlbuf* rlb)
{
    if (rlb == NULL) {
        errno = EINVAL;
        return -1;
    }

    rlb->size = 0;
    rlb->fd = fd;
    rlb->p = rlb->buf;
    return 0;
}


ssize_t
readLineBuf(struct rlbuf* rlb, void *buffer, size_t n)
{
    if (rlb == NULL || buffer == NULL || n <= 0) {
        errno = EINVAL;
        return -1;
    }

    char *buf = buffer;     /* Pointer arithmetic with void is disallowed */
    size_t totalRead = 0;

    /* Read until next new line character */
    for (;;) {
        if (rlb->p >= rlb->buf + rlb->size) {    /* Refill the buffer */
            ssize_t bytesRead;
            while ((bytesRead = read(rlb->fd, rlb->buf, sizeof(rlb->buf))) == -1) {
                if (errno == EINTR)
                    continue;                   /* Interrupted --> Restart */
                return -1;
            }
            if (bytesRead == 0)                 /* EOF */
                break;
            rlb->p = buf;
            rlb->size = bytesRead;
        }
        char ch = *rlb->p++;
        if (totalRead < n - 1) {                /* Copy next character */
            *buf++ = ch;
            totalRead++;
        }                                       /* Consume extra characters in long lines */

        if (ch == '\n')
            break;
    }

    *buf = '\0';
    return totalRead;
}

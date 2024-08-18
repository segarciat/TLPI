#ifndef _READLINE_BUF_H
#define _READLINE_BUF_H

#include <sys/types.h>  /* size_t */

#define _RLBUF_SIZE 4096

struct rlbuf {
    int fd;
    char buf[_RLBUF_SIZE];
    size_t size;
    char* p;
};

/* Initializes the readLineBuffer. On success, returns 0. On failure, returns -1 and sets errno. */
int readLineBufInit(int fd, struct rlbuf* rlb);

/* Places the next line pointed to by rlbuf into the given buffer. At most n-1 bytes from the next
   line are placed in the buffer, and the line is null-terminated. If no EOF or newline is encountered
   within the first n-1 reads, the function continues to read from rlbuf, but does not store these bytes
   in the result buffer. On success, returns the number of characters placed in the buffer. On failure,
   returns -1. */
ssize_t readLineBuf(struct rlbuf* rlb, void *buffer, size_t n);

#endif

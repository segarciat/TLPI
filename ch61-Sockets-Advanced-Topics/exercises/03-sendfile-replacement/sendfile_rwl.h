#ifndef _SENDFILE_SOCKETS_H
#define _SENDFILE_SOCKETS_H

#include <sys/types.h>  /* ssize_t, off_t, size_t */

/*  Transfer contents from in_fd to out_fd. Expects in_fd to be a file descriptor
    to a regular file, and out_fd to be a file descriptor referring to a socket.

    If offset is not NULL, it points to a file offset from which count bytes are
    transferred. The file offset remains unchanged, and the offset pointer is updated
    to the position of the byte after the last read is it is a value-result argument).
   
    If offset is NULL, then data is transferred from the current file offset, and
    the file offset is updated accordingly.

    See sendfile (2) for details about the operation of this function */
ssize_t sendfile_rwl(int out_fd, int in_fd, off_t *offset, size_t count);

#endif

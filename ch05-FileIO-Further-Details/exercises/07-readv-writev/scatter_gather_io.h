#ifndef SCATTER_GATHER_IO_H
#include <sys/types.h>  // size_t, ssize_t

struct my_iovec {
    void *iov_base;
    size_t iov_len;
};

ssize_t my_readv(int fd, const struct my_iovec *iov, int iovcnt);

#endif

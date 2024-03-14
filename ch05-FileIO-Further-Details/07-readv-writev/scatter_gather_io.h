#include <unistd.h> // size_t, ssize_t

struct iovec {
    void *iov_base;
    size_t iov_len;
};


ssize_t readv(int fd, const struct iovec *iov, int iovcnt);

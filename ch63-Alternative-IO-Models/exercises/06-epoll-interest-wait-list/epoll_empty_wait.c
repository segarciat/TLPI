#include <sys/epoll.h>
#include "tlpi_hdr.h"

int
main(int argc, char *argv[])
{
    int epfd = epoll_create(1);
    if (epfd == -1)
        errExit("epoll_create(): Failed to create an epoll structure");

    int ready;
    struct epoll_event epev;
    ready = epoll_wait(epfd, &epev, 1, 0);
    if (ready == -1)
        errExit("epoll_wait(): Error while waiting on empty epoll list");
    printf("ready: %d\n", ready);
    exit(EXIT_SUCCESS);
}

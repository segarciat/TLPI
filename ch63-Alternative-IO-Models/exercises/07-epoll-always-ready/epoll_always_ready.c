#include <sys/epoll.h>
#include <string.h>     /* strcmp() */
#include <errno.h>      /* errno, EEINTR */
#include <unistd.h>     /* close(), pipe(), write() */
#include <stdio.h>      /* printf(), NULL */
#include <stdlib.h>     /* calloc() */
#include <sys/types.h>  /* size_t */
#include "tlpi_hdr.h"

#define MAX_POLLS 1000000

int
main(int argc, char *argv[])
{
    if (argc < 2 || argc > 3 || strcmp("--help", argv[1]) == 0)
        usageErr("%s numPipes [numPolls]\n"
                "\tnumPipes: Number of descriptors.\n"
                "\tnumPolls: Number of polls to perform (default %lu).\n", argv[0], (size_t) MAX_POLLS);

    size_t numPipes = getLong(argv[1], GN_GT_0, "numPipes");
    size_t numPolls = (argc == 3) ? getLong(argv[2], GN_GT_0, "numPolls") : MAX_POLLS;

    /* Create epoll */
    int epfd = epoll_create(numPipes);
    if (epfd == -1)
        errExit("epoll_create(): Failed to create epoll structure");

    /* Create pipes, write to them, and add them to epoll interest list */
    int (*pfds)[2] = calloc(numPipes, sizeof(int[2]));
    if (pfds == NULL)
        errExit("malloc(): Failed to allocate memory for pipe descriptors");

    for (size_t i = 0; i < numPipes; i++) {
        if (pipe(pfds[i]) == -1)
            errExit("pipe(): Failed to create pipe");
        int ch = 'x';
        if (write(pfds[i][1], &ch, 1) == -1)
            fatal("write(): Failed to write to pipe");
        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.ptr = pfds[i];
        if (epoll_ctl(epfd, EPOLL_CTL_ADD, pfds[i][0], &ev) == -1)
            errExit("epoll_ctl(): Failed to add read end of pipe to interest list");
    }

    /* Monitor files descriptors */
    int ready;
    while (numPipes > 0 && numPolls > 0) {
        numPolls--;
        struct epoll_event evlist;
        ready = epoll_wait(epfd, &evlist, /* 1 event */ 1, -1);
        if (ready == -1) {
            if (errno == EINTR)
                continue;
            else
                errExit("epoll_wait(): Error while waiting on file descriptor");
        }
        int *pfd = (int *) evlist.data.ptr;
        if (evlist.events & EPOLLIN) {
            printf("ready: fd %d\n", pfd[0]);
        } else if (evlist.events & (EPOLLHUP || EPOLLERR)) {
            printf("closing: %d\n", evlist.data.fd);
            if (close(pfd[0]) == -1 || close(pfd[1]) == -1)
                errExit("close(): Failed to close pipe");
            numPipes--;
        }
    }
    exit(EXIT_SUCCESS);
}

/* Modified by Sergio E. Garcia Tapia for Exercise 63-5 */

/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2023.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 63-9 */

/*  self_pipe.c

   Employ the self-pipe trick so that we can avoid race conditions while both
   polling on a set of file descriptors and also waiting for a signal.

   Usage as shown in synopsis below; for example:

        self_pipe - 0
*/
#include <sys/time.h>
#include <poll.h>
#include <fcntl.h>
#include <signal.h>
#include "tlpi_hdr.h"

static int pfd[2];                      /* File descriptors for pipe */

static void
handler(int sig)
{
    int savedErrno;                     /* In case we change 'errno' */

    savedErrno = errno;
    if (write(pfd[1], "x", 1) == -1 && errno != EAGAIN)
        errExit("write");
    errno = savedErrno;
}

int
main(int argc, char *argv[])
{
    if (argc < 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s {timeout|-} fd...\n"
                "\t\t('-' means infinite timeout)\n", argv[0]);

    int timeout = (strcmp(argv[1], "-") == 0) ?
                -1 : getLong(argv[1], 0, "timeout") * 1000;

    /* Build the pollfd structures from the fd numbers given in command line */
    int nfds = argc - 2 + 1;    /* 1 extra for self-pipe */
    struct pollfd* pollFds = malloc(nfds);   /* 1 extra for self-pipe */
    if (pollFds == NULL)
        errExit("malloc(): Failed to allocate space for pollfd structures");

    for (int j = 2; j < argc; j++) {
        pollFds[j - 2].fd = getInt(argv[j], 0, "fd");
        pollFds[j - 2].events = POLLIN;
    }

    /* Create pipe before establishing signal handler to prevent race */

    if (pipe(pfd) == -1)
        errExit("pipe");
    pollFds[nfds - 1].fd = pfd[0];      /* Add read end of pipe to 'readfds' */
    pollFds[nfds - 1].events = POLLIN;

    /* Make read and write ends of pipe nonblocking */

    int flags = fcntl(pfd[0], F_GETFL);
    if (flags == -1)
        errExit("fcntl-F_GETFL");
    flags |= O_NONBLOCK;                /* Make read end nonblocking */
    if (fcntl(pfd[0], F_SETFL, flags) == -1)
        errExit("fcntl-F_SETFL");

    flags = fcntl(pfd[1], F_GETFL);
    if (flags == -1)
        errExit("fcntl-F_GETFL");
    flags |= O_NONBLOCK;                /* Make write end nonblocking */
    if (fcntl(pfd[1], F_SETFL, flags) == -1)
        errExit("fcntl-F_SETFL");

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;           /* Restart interrupted reads()s */
    sa.sa_handler = handler;
    if (sigaction(SIGINT, &sa, NULL) == -1)
        errExit("sigaction");

    int ready;
    while ((ready = poll(pollFds, nfds, timeout)) == -1 && errno == EINTR)
        continue;                       /* Restart if interrupted by signal */
    if (ready == -1)                    /* Unexpected error */
        errExit("poll()");

    if (pollFds[nfds - 1].revents & POLLIN) {   /* Handler was called */
        printf("A signal was caught\n");

        for (;;) {                      /* Consume bytes from pipe */
            char ch;
            if (read(pfd[0], &ch, 1) == -1) {
                if (errno == EAGAIN)
                    break;              /* No more bytes */
                else
                    errExit("read");    /* Some other error */
            }

            /* Perform any actions that should be taken in response to signal */
        }
    }

    /* Examine file descriptor sets returned by poll() to see
       which other file descriptors are ready */

    printf("ready = %d\n", ready);
    for (int j = 0; j < nfds - 1; j++)
        printf("%d: %s\n", pollFds[j].fd, (pollFds[j].revents & POLLIN) ? "r" : "");

    /* And check if read end of pipe is ready */

    printf("%d: %s   (read end of pipe)\n", pfd[0],
            (pollFds[nfds - 1].revents & POLLIN) ? "r" : "");

    if (ready == 0)
        printf("timeout after poll()\n");

    exit(EXIT_SUCCESS);
}

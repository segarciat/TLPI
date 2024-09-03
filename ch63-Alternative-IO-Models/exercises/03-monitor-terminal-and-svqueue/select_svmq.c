#include <unistd.h>         /* pipe(), _exit(), close() */
#include <sys/types.h>      /* For portability */
#include <sys/ipc.h>        /* For portability */
#include <sys/msg.h>        /* key_t, msgget(), msgctl() */
#include <sys/stat.h>       /* S_IRUSR, S_IWUSR */
#include <stdio.h>          /* printf(), fprintf(), stderr */
#include <stdlib.h>         /* atexit(), exit(), EXIT_SUCCESS, EXIT_FAILURE */
#include <stddef.h>         /* NULL */
#include <sys/time.h>       /* For portability */
#include <sys/select.h>     /* select() */
#include <sys/wait.h>       /* waitpid() */
#include <signal.h>         /* signal(), SIG_IGN, SIG_ERR, SIGPIPE */
#include "tlpi_hdr.h"       /* errExit() */
#include "select_svmq.h"    /* MAX_MQTEXT, struct mqbuf */

#define BUF_SIZE BUFSIZ

static int msqid;
static int exitHandlerCalled = 0;

static void
removeQueue()
{
    if (exitHandlerCalled)
        return;
    exitHandlerCalled = 1;
    printf("Removing System V message queue with ID: %d\n", msqid);
    if (msgctl(msqid, IPC_RMID, NULL) == -1)
        fprintf(stderr, "Failed to remove System V message queue with ID: %d (%s)\n", msqid, strerror(errno));
    else
        printf("Successfully removed message queue with ID: %d\n", msqid);
}

int
main(int argc, char *argv[])
{
    /* Create pipe */
    int pfds[2];
    if (pipe(pfds) == -1)
        errExit("pipe(): Failed to create pipe");

    /* Create a System V message queue */
    msqid = msgget(IPC_PRIVATE, S_IRUSR | S_IWUSR);
    if (msqid == -1)
        errExit("msgget(): Failed to create System V message queue");
    printf("Created message queue with ID: %d\n", msqid);

    if (atexit(removeQueue) == -1)
        errExit("atexit(): Failed to install exit handler");

    char buf[BUF_SIZE];

    /* Fork a child */
    switch (fork()) {
        case -1:
            errExit("fork(): Failed to create child process");
        case  0:    /* Child */
            /* Close unused pipe descriptor */
            if (close(pfds[0]) == -1)
                errExit("close(): Failed to close child's read end of pipe");

            if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
                errExit("signal(): Failed to ignore SIGPIPE disposition in child");

            struct mqbuf svmqbuf;
            ssize_t bytesRead;
            for (;;) {
                /* Read message from queue */
                bytesRead = msgrcv(msqid, &svmqbuf, MAX_MQTEXT, /* Remove first message */ 0, /* No flags */ 0);
                if (bytesRead == -1) {
                    if (errno == EINTR)
                        continue;
                    if (errno == EIDRM)
                        break;
                    errExit("msgrcv(): Failed to read message from System V message queue");
                }
                if (bytesRead == 0)
                    break;
                printf("Child read %lld bytes from System V message queue, writing to pipe\n", (long long) bytesRead);

                /* Write message to pipe */
                if (write(pfds[1], svmqbuf.mqtext, bytesRead) == -1) {
                    if (errno == EPIPE)
                        break;
                    fatal("write(): Failed to write System V message queue message to pipe");
                }
            }
            if (close(pfds[1]) == -1)
                errExit("close(): Child failed to close write end of pipe");
            _exit(EXIT_SUCCESS);
        default:    /* Parent */
            if (close(pfds[1]) == -1)
                errExit("close(): Failed to close parent's write end of pipe");
            fd_set readfds;
            int nfds = pfds[0] + 1;
            for (;;) {
                FD_ZERO(&readfds);
                FD_SET(STDIN_FILENO, &readfds);
                FD_SET(pfds[0], &readfds);
                int ready = select(nfds, &readfds, NULL, NULL, NULL);
                if (ready == -1) {
                    if (errno  == EINTR)
                        continue;
                    errExit("select(): Error while waiting for I/O");
                }
                ssize_t bytesRead;
                if (FD_ISSET(STDIN_FILENO, &readfds)) {
                    while ((bytesRead = read(STDIN_FILENO, buf, BUF_SIZE)) == -1 && errno == EINTR)
                        continue;
                    if (bytesRead == -1)
                        errExit("read(): Failed while reading from stdin");
                    if (bytesRead == 0)     /* EOF */
                        break;
                    printf("Parent read %lld bytes from stdin\n", (long long) bytesRead);
                }
                if (FD_ISSET(pfds[0], &readfds)) {        
                    while ((bytesRead = read(pfds[0], buf, BUF_SIZE)) ==-1 && errno == EINTR)
                        continue;
                    if (bytesRead == -1)
                        errExit("read(): Failed while reading from pipe");
                    if (bytesRead == 0)     /* Child closed pipe */
                        break;
                    printf("Parent read %lld bytes from pipe\n", (long long) bytesRead);
                }
            }
            /* Ensure child sees EPIPE */
            if (close(pfds[0]) == -1)
                errExit("close(): Failed to close parent's read end of pipe");
            removeQueue();
            while (waitpid(-1, NULL, 0) == -1 && errno == EINTR)
                continue;
            exit(EXIT_SUCCESS);
    }
}

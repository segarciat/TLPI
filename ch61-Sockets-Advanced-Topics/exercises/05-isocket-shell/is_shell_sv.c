#include "is_shell.h"
#include <sys/wait.h>   /* waitpid() */
#include <signal.h>     /* sigaction() */

#define BACKLOG 5
#define GENERIC_ERROR_MESSAGE "Internal server error"

static void /* Reap children processes */
sigchldHandler(int sig)
{
    int savedErrno = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0)
        continue;
    errno = savedErrno;
}

static void
handleRequest(int cfd, struct sockaddr* clAddr, socklen_t clAddrLen)
{
    char command[MAX_COMMAND_SIZE + 1];
    char clAddrStr[IS_ADDR_STR_LEN];

    /* Read command sent by client */
    inetAddressStr(clAddr, clAddrLen, clAddrStr, IS_ADDR_STR_LEN);
    ssize_t bytesRead = readn(cfd, command, MAX_COMMAND_SIZE);
    printf("%s requested command: %s\n", clAddrStr, command);
    if (bytesRead == 0) {   /* EOF */
        close(cfd);
        exit(EXIT_SUCCESS);
    }

    /* Duplicate socket on stdout and stderr, and close spare connected descriptor */
    if (
        bytesRead == -1 ||
        (cfd != STDOUT_FILENO && dup2(cfd, STDOUT_FILENO) == -1) ||
        (cfd != STDERR_FILENO && dup2(cfd, STDERR_FILENO) == -1) ||
        close(cfd) == -1
    ) {
        writen(cfd, GENERIC_ERROR_MESSAGE, sizeof(GENERIC_ERROR_MESSAGE));
        exit(EXIT_FAILURE);
    }
    command[bytesRead] = '\0';

    /* Exec a shell to handle command */
    execl("/bin/sh", "sh", "-c", command, (char *) NULL);
}

int
main(int argc, char *argv[])
{
    /* Install SIGCHLD handler to reap child processes */
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = sigchldHandler;
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
        errExit("sigaction(): Failed to install SIGCHLD handler");

    int lfd = inetListen(PORT_NUM, BACKLOG, NULL);
    if(lfd == -1)
        errExit("inetListen(): Failed to create listening socket on port %s", PORT_NUM);

    for (;;) {
        struct sockaddr_storage clAddr;
        socklen_t clAddrLen = sizeof(struct sockaddr_storage);
        int cfd = accept(lfd, (struct sockaddr *) &clAddr, &clAddrLen);
        if (cfd == -1) {
            errMsg("accept(): Failed to accept client connection; skipping");
            continue;
        }

        switch (fork()) {
            case -1:    /* Give up on client */
                errMsg("fork(): Failed to fork a child to handle request; skipping");
                close(cfd);
                break;
            case  0:    /* Child */
                close(lfd); /* Unneeded copy of listening socket */
                handleRequest(cfd, (struct sockaddr *) &clAddr, clAddrLen);
                _exit(EXIT_FAILURE);
            default:    /* Parent */
                close(cfd); /* Unneeded copy of connected socket */
                break;
        }
    }
}

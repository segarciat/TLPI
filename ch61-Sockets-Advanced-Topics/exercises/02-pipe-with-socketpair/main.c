#include "pipe_sp.h"
#include <stdlib.h>     /* exit(), EXIT_FAILURE, EXIT_SUCCESS */
#include <stdio.h>      /* fprintf(), stderr */
#include <unistd.h>     /* fork(), read(), write() */
#include <sys/types.h>  /* ssize_t */
#include <sys/wait.h>   /* waitpid() */
#include <string.h>     /* strerror(), strcmp() */
#include <errno.h>      /* errno */

#define BUF_SIZE 32

static void
errorExit(const char* msg)
{
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(EXIT_FAILURE);
}

static void
displayUsage(FILE* stream, const char* programName)
{
    fprintf(stream, "Usage: %s text\n", programName);
    fprintf(stream, "Echos the first command-line argument to standard output\n");
}

int
main(int argc, char *argv[])
{
    if (argc > 1 && strcmp("--help", argv[1]) == 0) {
        displayUsage(stdout, argv[0]);
        exit(EXIT_SUCCESS);
    }

    if (argc != 2) {
        displayUsage(stderr, argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Create pipe */
    int pfds[2];
    if (pipe_sp(pfds) == -1)
        errorExit("pipe_sp(): Failed to create pipe");
    
    /* Fork a child */
    switch (fork()) {
        case -1:
            errorExit("fork(): Failed to create child process");
        case  0:    /* Child */
            /* Close unused end of pipe (write end) */
            if (close(pfds[1]) == -1)
                errorExit("close() - Child failed to close write end of pipe");

            /* Until EOF, read all data from pipe and write it to stdout */
            char buf[BUF_SIZE];
            for (;;) {
                ssize_t bytesRead = read(pfds[0], buf, BUF_SIZE);
                if (bytesRead == -1)
                    errorExit("read() - Error reading from pipe");
                if (bytesRead == 0)  /* EOF */
                    break;

                if (write(STDOUT_FILENO, buf, bytesRead) != bytesRead) {
                    fprintf(stderr, "Unexpected error writing to stdout\n");
                    exit(EXIT_FAILURE);
                }
            }
            write(STDOUT_FILENO, "\n", 1);
            if (close(pfds[0]) == -1)
                errorExit("close() - Child failed to close read end of pipe");
            _exit(EXIT_SUCCESS);
        default:    /* Parent */
            /* Close unused (read) end of the file */
            if (close(pfds[0]) == -1)
                errorExit("close() - Parent failed to close read end of pipe");

            /* Write single argument to pipe */
            size_t argLen = strlen(argv[1]);
            if (write(pfds[1], argv[1], argLen) != argLen) {
                fprintf(stderr, "Unexpected error reading all data to pipe\n");
                exit(EXIT_FAILURE);
            }

            if (close(pfds[1]) == -1)
                errorExit("close() - Parent failed to close read end of pipe");
            
            /* Reap child */
            while (waitpid(-1, NULL, 0) == -1 && errno == EINTR)
                continue;
            if (errno == -1)
                errorExit("waitpid() - Error reaping child");
            exit(EXIT_SUCCESS);
    }
}

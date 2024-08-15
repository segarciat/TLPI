#include <sys/file.h>   /* flock() */
#include <string.h>     /* strcmp(), strerror() */
#include <stdio.h>      /* fprintf(), printf(), stderr */
#include <stdlib.h>     /* exit(), EXIT_FAILURE, EXIT_SUCCESS */
#include <errno.h>      /* errno */
#include <fcntl.h>      /* open(), O_* constants */
#include <signal.h>     /* sigaction, etc */
#include <unistd.h>     /* alarm(), getpid() */
#include <sys/wait.h>   /* waitpid() */

#define USAGE_FORMAT    "Usage: %s file-to-lock\n" \
                        "Demonstrates the release and inheriance semantics of flock()\n"
#define FLOCK_TIMEOUT   5 /* 5 seconds */

static void
errorExit(const char *msg)
{
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(EXIT_FAILURE);
}

static void
sigAlrmHandler(int sig)     /* Just interrupt flock() */
{
}

int
main(int argc, char *argv[])
{
    if (argc > 1 && strcmp("--help", argv[1]) == 0) {
        printf(USAGE_FORMAT, argv[0]);
        exit(EXIT_SUCCESS);
    }

    if (argc != 2) {
        fprintf(stderr, USAGE_FORMAT, argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Open input file and get an exclusive lock */
    int fd = open(argv[1], O_RDONLY);
    if (fd == -1)
        errorExit("Failed to open input file");

    if (flock(fd, LOCK_EX) == -1)
        errorExit("Failed to lock input file");

    /* Open the file a second time, creating a different file descriptor and open file description */
    int anotherFd = open(argv[1], O_RDONLY);
    if (anotherFd == -1)
        errorExit("Failed to obtain second file descriptor for input file");

    printf("Obtained first exclusive lock on %s\n", argv[1]);

    /* Set up alarm handler */
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = sigAlrmHandler;
    if (sigaction(SIGALRM, &sa, NULL) == -1)
        errorExit("Failed to install SIGALRM handler");

    /* Demonstrate that process can lock itself out of a file */
    printf("Attempting to obtain second exclusive lock on %s\n", argv[1]);
    alarm(FLOCK_TIMEOUT);
    if (flock(anotherFd, LOCK_EX) == -1) {
        if (errno == EINTR)
            printf("flock() timed out as expected while blocked waiting on exclusive lock\n");
        else
            errorExit("Failed to obtain second exclusive lock on input file");
    }

    int savedErrno = errno;
    alarm(0);
    errno = savedErrno;

    /* Demonstrate child can release the parent's lock */
    printf("[%ld] Forking child while still holding lock\n", (long) getpid());
    switch (fork()) {
        case -1:
            errorExit("Failed to fork a process");
        case  0:    /* Child */
            printf("[%ld] Child releasing inherited lock\n", (long) getpid());
            if (flock(fd, LOCK_UN) == -1)
                errorExit("Child failed to release inherited lock");
            _exit(EXIT_SUCCESS);
        default:    /* Parent */
            if (waitpid(-1, NULL, 0) == -1)
                errorExit("Failed to reap child");
            printf("[%ld] Reaped child, retrying to acquire lock with the second file description\n",
                        (long) getpid());
            if (flock(anotherFd, LOCK_EX) == -1)
                errorExit("Failed to acquire the second exclusive lock after fork");
            printf("[%ld] Successfully acquired the exclusive lock\n", (long) getpid());
            exit(EXIT_SUCCESS);
    }

    exit(EXIT_SUCCESS);
}

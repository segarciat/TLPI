#define _POSIX_C_SOURCE 200809L
#include <unistd.h>     /* getopt() */
#include <stdio.h>      /* printf(), fprintf(), stderr */
#include <stdlib.h>     /* strtol(), exit(), EXIT_FAILURE, EXIT_SUCCESS */
#include <errno.h>      /* errno */
#include <string.h>     /* strerror(errno) */
#include <time.h>       /* time_t, clock_gettime(), clock_nanlosleep() */
#include <fcntl.h>      /* open() */
#include <sys/stat.h>   /* Mode S_* constants */
#include <stdbool.h>    /* bool */

#define DEFAULT_WAIT_SECS 8
#define DEFAULT_RETRIES -1
#define RDONLY_PERMS (S_IRUSR | S_IRGRP | S_IROTH)

extern int optind, optopt;
extern char *optarg;

/* Displays a message and the message associated with errno's current value on stderr, then exits */
static void
errorExit(const char *msg)
{
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(EXIT_FAILURE);
}

/* Displays a usage message for the program */
static void
displayUsage(FILE* stream, const char *programName)
{
    fprintf(stream, "Usage: %s [OPTION]... FILE\n", programName);
    fprintf(stream, "Creates a file with the given name to be used as a semaphore.\n");
    fprintf(stream, "\n");
    fprintf(stream, "Options:\n\n");
    fprintf(stream, "  -w wait_time        If file cannot be locked, waits the specified number of seconds\n");
    fprintf(stream, "                      before retrying. Defaults to %ld seconds.\n", (long) DEFAULT_WAIT_SECS);
    fprintf(stream, "  -r max_retries      Makes at most max_retries + 1 attempts to lock the file, sleeping\n");
    fprintf(stream, "                      between attempts according to the wait_time. If max_retries is\n");
    fprintf(stream, "                      negative, retries forever. Default is %ld.\n", (long) DEFAULT_RETRIES);
    
}

/* Parse s as an integer. If valid, place the result in buffer n and return 0. Otherwise, return -1. */
static int
parseLong(const char *s, long* n)
{
    long m;
    char *endp;

    errno = 0;
    m = strtol(s, &endp, 0);
    if (errno != 0 || *endp != '\0')
        return -1;
    *n = m;
    return 0;
}

int
main(int argc, char* argv[])
{
    if (argc > 1 && strcmp("--help", argv[1]) == 0) {
        displayUsage(stdout, argv[0]);
        exit(EXIT_SUCCESS);
    }

    int opt;

    long retries = -1; /* Infinite */
    time_t waitSecs = DEFAULT_WAIT_SECS;

    while ((opt = getopt(argc, argv, "+:r:w:")) != -1) {
        switch (opt) {
            case 'w':
                long n;
                if (parseLong(optarg, &n) == -1 || n < 0) {
                    fprintf(stderr, "%s: Invalid sleep seconds: %s\n", argv[0], optarg);
                    exit(EXIT_FAILURE);
                }
                waitSecs = n;
                break;
            case 'r':
                if (parseLong(optarg, &retries) == -1) {
                    fprintf(stderr, "%s: Invalid number of retries: %s\n", argv[0], optarg);
                    exit(EXIT_FAILURE);
                }
                break;
            case ':':
                fprintf(stderr, "%s: Missing argument for -%c\n", argv[0], optopt);
                exit(EXIT_FAILURE);
            case '?':
                fprintf(stderr, "%s: Unrecognized option (-%c)\n", argv[0], optopt);
                exit(EXIT_FAILURE);
            default:
                fprintf(stderr, "%s: Unexpected switch case\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    if (optind != argc - 1) {
        displayUsage(stderr, argv[0]);
        exit(EXIT_FAILURE);
    }

    bool forever = retries < 0;
    struct timespec ts;
    int fd;
    for (;;) {
        /* Create file exclusive */
        fd = open(argv[optind], O_RDONLY | O_CREAT | O_EXCL, RDONLY_PERMS);
        if (fd == -1 && errno != EEXIST)
            errorExit("Failed to open file");
        if (fd != -1 || (!forever && retries-- <= 0))
            break;
        /* Sleep before retrying */
        if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
            errorExit("Failed to get current time");
        ts.tv_sec += waitSecs;
        int s = clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &ts, NULL);
        if (s != 0)
            errorExit("Failed to sleep full duration");
    }
    if (fd < 0) {
        fprintf(stderr, "%s: Gave up on locking %s\n", argv[0], argv[optind]);
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}

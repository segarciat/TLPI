#include <fcntl.h>      /* fcntl() */
#include <string.h>     /* strcmp(), strerror() */
#include <stdio.h>      /* printf(), fprintf(), stdout, stderr, FILE */
#include <stdlib.h>     /* exit(), EXIT_FAILURE, EXIT_SUCCESS, strtol() */
#include <errno.h>      /* errno */
#include <sys/types.h>  /* pid_t */
#include <unistd.h>     /* getpid() */

#define LOCK_ATTEMPTS 10000

static void
errorExit(const char *msg)
{
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(EXIT_FAILURE);
}

static void
usage(FILE* stream, const char* programName)
{
    fprintf(stream, "Usage: %s file-to-lock n\n", programName);
    fprintf(stream, "Makes %ld attempts to lock the (2n)-th byte of the given file.\n", (long) LOCK_ATTEMPTS);
    fprintf(stream, "  file-to-lock\n"
                    "    The file whose single byte we wish to lock.\n"
                    "  n\n"
                    "    Positive integer influencing the byte to be locked.\n");
}

/* Parse a string as an integer. If valid, places result in buffer n and returns 0. Otherwise returns -1 */
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
main(int argc, char *argv[])
{
    if (argc > 1 && strcmp("--help", argv[1]) == 0) {
        usage(stdout, argv[0]);
        exit(EXIT_SUCCESS);
    }

    if (argc != 3) {
        usage(stderr, argv[0]);
        exit(EXIT_FAILURE);
    }
    
    /* Ensure a non-negative integer is given */
    long n;
    if (parseLong(argv[2], &n) == -1 || n < 0) {
        fprintf(stderr, "Expected non-negative integer in argument 2, but got: %s\n", argv[2]);
        exit(EXIT_FAILURE);
    }

    /* Open given file */
    int fd = open(argv[1], O_RDWR);
    if (fd == -1)
        errorExit("Failed to open input file");

    /* Make repeated attemtps to lock the same byte */
    struct flock fl;
    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 2 * n;
    fl.l_len = 1;

    size_t successes = 0;
    pid_t pid = getpid();
    printf("[%ld] Proceeding to attempt to lock file %ld times\n", (long) pid, (long) LOCK_ATTEMPTS);
    for (size_t attempt = 0; attempt < LOCK_ATTEMPTS; attempt++) {
        if (fcntl(fd, F_SETLK, &fl) == -1 && errno != EAGAIN)
            errorExit("Failed to lock byte range");
        successes += (errno == 0);
    }

    printf("[%ld] Number of successful write lock acquisitions: %ld out of %ld\n",
        (long) pid, successes, (long) LOCK_ATTEMPTS);
    exit(EXIT_SUCCESS);
}

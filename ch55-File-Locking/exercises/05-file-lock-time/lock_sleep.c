#include <fcntl.h>      /* fcntl(), open() */
#include <stdio.h>      /* fprintf(), printf(), stderr, stdout, FILE */
#include <stdlib.h>     /* exit(), EXIT_FAILURE, EXIT_SUCCESS */
#include <string.h>     /* strcmp(), strerror() */
#include <errno.h>      /* errno */
#include <sys/types.h>  /* pid_t */
#include <unistd.h>     /* pause() */

#define MAX_LOCK_BYTE 80000

static void
errorExit(const char *msg)
{
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(EXIT_FAILURE);
}

static void
usage(FILE *stream, const char *programName)
{
    fprintf(stream, "Usage: %s file-to-lock\n", programName);
    fprintf(stream, "Given a file with at least %lld bytes, locks the file and sleeps\n",
              (long long) MAX_LOCK_BYTE + 1);
}

int
main(int argc, char *argv[])
{
    /* Require a file to lock as a command-line argument */
    if (argc > 1 && strcmp("--help", argv[1]) == 0) {
        usage(stdout, argv[0]);
        exit(EXIT_SUCCESS);
    }
    if (argc != 2) {
        usage(stderr, argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Lock even bytes of file, through position MAX_LOCK_BYTE*/
    int fd = open(argv[1], O_RDWR);
    if (fd == -1)
        errorExit("Failed to open input file");

    struct flock fl;
    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_len = 1;

    pid_t pid = getpid();
    printf("[%ld] Locking even bytes in file through position %lld\n", (long) pid, (long long) MAX_LOCK_BYTE);
    for (ssize_t bytePos = 0; bytePos <= MAX_LOCK_BYTE; bytePos += 2) {
        fl.l_start = bytePos;
        if (fcntl(fd, F_SETLK, &fl) == -1) {
            fprintf(stderr, "Error while locking byte %lld: %s\n", (long long) bytePos, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    /* Pause */
    printf("[%ld] Successfully locked bytes even byte positions through %lld\n",
                (long) pid, (long long) MAX_LOCK_BYTE);
    printf("[%ld] Pausing execution...\n", (long) pid);
    pause();

    /* Terminate */
    printf("[%ld] Exiting.\n", (long) pid);
    exit(EXIT_SUCCESS);
}

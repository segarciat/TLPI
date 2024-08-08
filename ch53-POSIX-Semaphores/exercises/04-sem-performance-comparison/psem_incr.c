#include "util.h"
#include <fcntl.h>      /* Defines O_* constants */
#include <sys/stat.h>   /* Defines mode constants */
#include <string.h>     /* strcmp() */
#include <stdlib.h>     /* exit(), EXIT_FAILURE, EXIT_SUCCESS */
#include <stdio.h>      /* fprintf(), printf(), stderr, stdout */
#include <unistd.h>     /* getpid() */
#include <semaphore.h>  /* sem_t, sem_open(), sem_wait(), sem_post(), SEM_FAILED */

int
main(int argc, char* argv[])
{
    if (argc  > 1 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "h") == 0)) {
        fprintf(stdout, USAGE, argv[0]);
        exit(EXIT_SUCCESS);
    }

    if (argc != 3) {
        fprintf(stderr, USAGE, argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Open an existing semaphore */
    sem_t* sem = sem_open(argv[1], O_RDWR);
    if (sem == SEM_FAILED)
        errorExit(true, "%s: Failed to open semaphore %s", argv[0], argv[1]);

    /* Parse given count */
    long n;
    if (parseLong(argv[2], &n) == -1)
        errorExit(false, "%s: Failed to parse %s", argv[0], argv[2]);

    if (n <= 0) {
        fprintf(stderr, "%s: Expected a positive count, but got %ld\n", argv[0], n);
        exit(EXIT_FAILURE);
    }
    
    /* Increment and decrement the specified number of times */
    for (long m = 0; m < n; m++) {
        if (sem_post(sem) == -1)
            errorExit(true, "%s: Failed to post to semaphore %s", argv[0], argv[1]);
        if (sem_wait(sem) == -1)
            errorExit(true, "%s: Failed to wait for semaphore %s", argv[0], argv[1]);
    }
    printf("[PID %ld]: Posted to and wait for semaphore a total of %ld time%s\n",
            (long) getpid(), n, n == 1 ? "" : "s");
    exit(EXIT_SUCCESS);
}

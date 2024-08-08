#include "util.h"
#include <stdio.h>      /* fprintf() */
#include <string.h>     /* strcmp() */
#include <stdlib.h>     /* exit(), EXIT_FAILURE, EXIT_SUCCESS */
#include <unistd.h>     /* getpid() */
#include <sys/types.h>  /* For portability */
#include <sys/ipc.h>    /* For portability */
#include <sys/sem.h>    /* semop() */
#include <limits.h>     /* INT_MAX */

int
main(int argc, char* argv[])
{
    /* Display usage message if necessary */
    if (argc > 1 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)) {
        fprintf(stdout, USAGE, argv[0]);
        exit(EXIT_SUCCESS);
    }
    
    if (argc != 3) {
        fprintf(stderr, USAGE, argv[0]);
        exit(EXIT_SUCCESS);
    }

    /* Parse semaphore id */
    long semidLong;
    if (parseLong(argv[1], &semidLong) == -1) {
        errorExit(false, "%s: Failed to parse semaphore ID from %s", argv[0], argv[1]);
    }

    if (semidLong < 0 || semidLong > INT_MAX)
        errorExit(false, "%s: Invalid semaphore ID %s\n", argv[0], argv[1]);

    int semid = (int) semidLong;
    /* Parse integer count */
    long n;
    if (parseLong(argv[2], &n) == -1)
        errorExit(false, "%s: Failed to parse integer from: %s\n", argv[0], argv[2]);

    if (n <= 0)
       errorExit(stderr, "%s: Expected positive integer for count, but got: %ld\n", argv[0], n);

    struct sembuf sop;
   
    sop.sem_num = 0; /* Semaphore 0 (first semaphore in the set) */
    sop.sem_flg = 0; /* No flags */
    for (long m = 0; m < n; m++) {
       /* Increment */
       sop.sem_op =  1;
       if (semop(semid, &sop, /* One operation */ 1) == -1)
           errorExit(stderr, "%s: Failed to increment semaphore value", argv[1]);

       /* Decrement */
       sop.sem_op = -1;
       if (semop(semid, &sop, /* One operation */ 1) == -1)
           errorExit(stderr, "%s: Failed to decrement semaphore value", argv[1]);
    }

    printf("[PID %ld] Incremented and decremented value of semaphore a total of %ld time%s\n",
                (long) getpid(), n, (n == 1) ? "" : "s");

    exit(EXIT_SUCCESS);
}

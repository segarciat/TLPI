#include <semaphore.h>
#include <sys/file.h>   /* flock() */
#include <stdio.h>      /* fprintf(), printf(), stderr, vfprintf() */
#include <stdlib.h>     /* exit(), EXIT_SUCCESS, EXIT_FAILURE */
#include <fcntl.h>      /* open(), O_* constants */
#include <unistd.h>     /* close(), alarm() */
#include <string.h>     /* strcmp(), strerror() */
#include <stdbool.h>    /* bool, FALSE, TRUE */
#include <stdarg.h>     /* va_list, va_start(), va_end() */
#include <sys/mman.h>   /* mmap() */
#include <errno.h>      /* errno */
#include <signal.h>     /* struct sigaction, sigemptyset(), sigaction(), SIGALARM */
#include <sys/wait.h>   /* wait() */


#define USAGE_FMT   "Usase: %s file_1 file_2\n" \
                    "Synopsis: Demonstrates whether flock() auto detects simple deadlocks\n"
#define LOCK_TIMEOUT 5

static void
errorExit(bool showErrno, const char *fmt, ...)
{
    if (fmt != NULL) {
        va_list varArgp;
        va_start(varArgp, fmt);
        vfprintf(stderr, fmt, varArgp);
        va_end(varArgp);
    }
    if (showErrno)
        fprintf(stderr, "%s%s", (fmt != NULL) ? ": " : "", strerror(errno));
    fprintf(stderr, "\n");
    exit(EXIT_FAILURE);
}

static void
sigAlrmHandler()        /* Just interrupt */
{
}

int
main(int argc, char* argv[])
{
    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        printf(USAGE_FMT, argv[0]);
        exit(EXIT_SUCCESS);
    }
    if (argc != 3)  {
        fprintf(stderr, USAGE_FMT, argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Created shared anonymous mapping for shared semaphores */
    char *mapAddr = mmap(NULL, 2 * sizeof(sem_t), PROT_READ| PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (mapAddr == MAP_FAILED)
        errorExit(true, "Failed to create shared anonymous mapping");

    /* Created in-use unnamed semaphores */
    sem_t* parentSem = (sem_t*) mapAddr;
    sem_t* childSem = (sem_t*) mapAddr + sizeof(sem_t);

    if (sem_init(parentSem, 1 /* Any nonzero */, 0) == -1)
        errorExit(true, "Failed to create unnamed semaphore for parent");
    if (sem_init(childSem, 1 /* Any nonzero */, 0) == -1)
        errorExit(true, "Failed to create unnamed semaphore for child");

    setbuf(stdout, NULL);
    pid_t childPid = fork();
    if (childPid == -1)
        errorExit(true, "Failed to fork child");

    /* Open distinct file descriptions for the same file in both parent and child */
    int fd1 = open(argv[1], O_RDONLY);
    if (fd1 == -1)
        errorExit(true, "Failed to open %s", argv[1]);
    int fd2 = open(argv[2], O_RDONLY);
    if (fd2 == -1)
        errorExit(true, "Failed to open %s", argv[2]);

    if (childPid == 0) {    /* Child */
        if (flock(fd2, LOCK_EX) == -1) {
            sem_post(parentSem);
            errorExit(true, "Child failed to lock %s", argv[2]);
        }
        if (sem_post(parentSem) == -1)
            errorExit(true, "Failed to post to parent sem from child");
        if (sem_wait(childSem) == -1)
            errorExit(true, "Child failed while waiting on semaphore");

        /* Install SIGALRM handler */
        struct sigaction sa;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        sa.sa_handler = sigAlrmHandler;
        if (sigaction(SIGALRM, &sa, NULL) == -1)
            errorExit(true, "Parent failed to install SIGALRM handler");

        alarm(LOCK_TIMEOUT);

        printf("Child owns lock on %s, waiting to acquire lock on %s\n", argv[2], argv[1]);
        if (flock(fd1, LOCK_EX) == -1) {
            if (errno == EINTR) {
                printf("Child requested to wait for lock timed out, exiting\n");
                _exit(EXIT_SUCCESS);
            }
            errorExit(true, "Child failed to acquire lock on file %s", argv[1]);
        }
        printf("Child unexpectedly acquired lock\n");    /* Should never reach here due to deadlock */
        _exit(EXIT_FAILURE);
    } else {                /* Parent */
        if (flock(fd1, LOCK_EX) == -1) {
           sem_post(childSem);
           errorExit(true, "Parent failed to lock %s", argv[1]);
        }
        printf("Parent has lock on %s, waiting to acquire lock on %s\n", argv[1], argv[2]);
        if (sem_post(childSem) == -1) 
            errorExit(true, "Failed to post to child sem from parent");
        if (sem_wait(parentSem) == -1)
            errorExit(true, "Parent failed while waiting on semaphore");
        
        if (flock(fd2, LOCK_EX) == -1)
            errorExit(true, "Parent failed to acquire lock on file %s", argv[2]);
        
        printf("Parent acquired lock on %s, waiting to reap child\n", argv[2]);
        if (wait(NULL) == -1)
            errorExit(true, "Failed to reap child");
        sem_destroy(parentSem);
        sem_destroy(childSem);
        munmap(mapAddr, 2 * sizeof(sem_t));
        exit(EXIT_SUCCESS);
    }
}

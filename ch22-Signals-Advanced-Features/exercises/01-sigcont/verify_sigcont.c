#define _DEFAULT_SOURCE /* Expose definitions of signal interface */
#include <stdio.h>      /* fprintf(), stderr, printf(), NULL */
#include <stdlib.h>     /* exit(), EXIT_FAILURE */
#include <signal.h>     /* struct sigaction, sigemptyset(), sigaction(), SIGCONT, sigaddset(), sigset_t */
#include <unistd.h>     /* pause(), getpid() */
#include "tlpi_hdr.h"

static void
sigcontHandler(int signal)
{
    (void) signal;
    /* UNSAFE: printf() is a non-async-signal-safe function */
    printf("Handled SIGCONT\n");
}

int
main(int argc, char *argv[])
{
    (void) argc;
    /* Establish disposition (handler) for SIGCONT */
    struct sigaction sa;
    
    if (sigemptyset(&sa.sa_mask) == -1)
        errExit("%s: Failed to set signal mask for SIGCONT handler", argv[0]);
    sa.sa_flags = 0;
    sa.sa_handler = sigcontHandler;
    if (sigaction(SIGCONT, &sa, NULL) == -1)
        errExit("%s: Failed to change SIGCONT disposition: handler not established", argv[0]);

    /* Add SIGCONT to process signal mask */
    sigset_t blockSet;
    if (sigemptyset(&blockSet) == -1)
        errExit("%s: Failed to initialize signal block set", argv[0]);
    if (sigaddset(&blockSet, SIGCONT) == -1)
        errExit("%s: Failed to add SIGCONT to block set", argv[0]);
    if (sigprocmask(SIG_BLOCK, &blockSet, NULL) == -1)
        errExit("%s: Failed to set disposition for SIGCONT", argv[0]);

    printf("PID: %ld\n", (long) getpid());
    printf("Blocking and handling SIGCONT\n");
    int remaining = 10;
    printf("Unblocking SIGCONT in: %d\n", remaining);
    while (remaining > 0) {
        sleep(1);
        printf("%d\n", --remaining);
    }
    if (sigprocmask(SIG_UNBLOCK, &blockSet, NULL) == -1)
        errExit("%s: Failed to unblock SIGCONT", argv[0]);
    printf("Exiting\n");
    
    exit(EXIT_SUCCESS);
}

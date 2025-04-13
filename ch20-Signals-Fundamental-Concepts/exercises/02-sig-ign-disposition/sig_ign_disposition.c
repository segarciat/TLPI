/**
 * @author: Sergio E. Garcia Tapia
 */

#define _GNU_SOURCE /* for strsignal() */
#include <string.h>
#include <signal.h> /* sigaction(), struct sigaction, SIG_IGN, SIG_DFL, sigemptyset() */
#include <stdio.h>  /* fprintf(), stderr */
#include <stdlib.h> /* exit(), EXIT_FAILURE */
#include <unistd.h> /* sleep() */
#include "signal_functions.h"

/**
 * Exercise 20.2 in TLPI.
 * 
 * Demonstrates that when the disposition of a pending signal is
 * changed to SIG_IGN, the program never sees the signal.
 *
 * I did this by setting the disposition of SIGINT to SIG_IGN,
 * suspending the process for 5 seconds so that I could send
 * SIGINT from the command-line (with CTRL+C or the kill command),
 * then displaying the set of pending signals after sleeping is over.
 * The pending set will be empty since SIG_IGN instructs the kernel
 * to discard the signals. Finally, I set the disposition of SIGINT
 * to SIG_DFL so that the default action takes place next time the
 * process receives SIGINT (process terminates).
 */
int
main(int argc, char *argv[])
{
    /* Shut compiler up :) */
    (void) argc;
    /* Set disposition of SIGINT to ignore */
    struct sigaction sa;
    if (sigemptyset(&sa.sa_mask) == -1) {
        fprintf(stderr, "%s: Failed to set mask for signal action\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    int signal = SIGINT;
    char *signalDescription = strsignal(signal);
    if (sigaction(signal, &sa, NULL) == -1) {
        fprintf(stderr, "%s: Failed to set disposition of %s\n", argv[0], signalDescription);
        exit(EXIT_FAILURE);
    }

    /* Idle process for a short duration */
    unsigned int duration = 5;
    printf("%s ignored. Sleeping for about %u seconds...\n", signalDescription, duration);
    fflush(stdout);
    sleep(duration);

    /* Verify signals were never seen */
    printf("Done sleeping! Checking signals in pending set.\n");
    printPendingSigs(stdout, "Pending signals:\n");

    /* Reset signal disposition and suspend until it is issued */
    printf("Resetting disposition of %s\n", signalDescription);
    sa.sa_handler = SIG_DFL;
    if (sigaction(signal, &sa, NULL) == -1) {
        fprintf(stderr, "%s: Failed to set disposition of %s\n", argv[0], signalDescription);
        exit(EXIT_FAILURE);
    }

    printf("Pausing until interrupted... (Press CTRL+C)\n");
    pause();
    exit(EXIT_SUCCESS);
}

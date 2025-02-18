#include <string.h> // strlen
#include <stdio.h>  // printf, fgets, fflush
#include <unistd.h> // sysconf
#include <stdlib.h> // exit, EXIT_FAILURE
#include <stddef.h> // size_t
#include "my_getpwnam.h"
#include "tlpi_hdr.h"

#define MAX_LOGIN_LEN 256

int
main()
{
    /* Allocate space for name */
    long lnmax = sysconf(_SC_LOGIN_NAME_MAX);
    if (lnmax == -1)
        lnmax = MAX_LOGIN_LEN;
    char *username = malloc((size_t) lnmax);
    if (username == NULL)
        errExit("malloc");

    /* Prompt for name */
    printf("Username: ");
    fflush(stdout);
    if (fgets(username, (int) lnmax, stdin) == NULL)
        fatal("failed to read name");
    size_t len = strlen(username);
    if (username[len-1] == '\n') /* Remove trailing newline, if any */
        username[len-1] = '\0';
    
    struct passwd* pwrecord =  my_getpwnam(username);
    if (pwrecord == NULL)
        errExit("Failed to get password record");

    printf("Found: %s with UID %ld\n", pwrecord->pw_name, (long) pwrecord->pw_uid);
    exit(EXIT_SUCCESS);
}

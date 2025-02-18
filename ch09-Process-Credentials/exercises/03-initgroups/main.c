#include <stdio.h>
#include <pwd.h>
#include <unistd.h>
#include "my_initgroups.h"
#include "tlpi_hdr.h"


/*
 * Expects a single command-line argument that is the login
 * name in this system. Displays the group list for this
 * process, then initializes the group list of the process
 * to the group list of the provided user.
 * Finally, it displays the group list after the change occurs.
 */
int main(int argc, char *argv[])
{
    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s login\n", argv[0]);

    const char *user = argv[1];

    struct passwd *pw = getpwnam(user);
    if (pw == NULL)
        fatal("Failed to find %s", user);

    long maxGroupLen = sysconf(_SC_NGROUPS_MAX);
    if (maxGroupLen == -1)
        fatal("Failed to get max group length");

    gid_t *groups = malloc((size_t) maxGroupLen * sizeof(*groups));
    if (groups == NULL)
        errExit("malloc");

    int lenGroups = getgroups((int) maxGroupLen, groups);
    if (lenGroups == -1) {
        free(groups);
        errExit("getgroups");
    }

    printf("Groups before:\n");
    for (int i = 0; i < lenGroups; i++) { 
        printf("%ld\n", (long) groups[i]);
    }

    if (my_initgroups(user, pw->pw_gid) == -1) {
        free(groups);
        errExit("Failed to set groups");
    }

    lenGroups = getgroups((int) maxGroupLen, groups);
    if (lenGroups == -1) {
        free(groups);
        errExit("getgroups");
    }

    printf("Groups after:\n");
    for (int i = 0; i < lenGroups; i++) { 
        printf("%ld\n", (long) groups[i]);
    }

    free(groups);
    exit(EXIT_SUCCESS);
}

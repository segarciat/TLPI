#define _POSIX_C_SOURCE 200809L /* Expose readlink() */
#include "tlpi_hdr.h"
#include <sys/types.h>          // pid_t
#include <linux/limits.h>       // PATH_MAX
#include <unistd.h>             // readlink()
#include <dirent.h>             // DIR, opendir(), readdir()
#include <string.h>             // strcmp
#include <stdlib.h>             // realpath()
#include <errno.h>              // errno
#include <ctype.h>              // isdigit()

int
main(int argc, char *argv[])
{
    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s file\n", argv[0]);

    /* Resolve argument's path */
    char path[PATH_MAX];
    if (realpath(argv[1], path) == NULL)
        errExit("realpath of %s", argv[1]);

    /* Open proc file system */
    DIR *procdir = opendir("/proc");
    if (procdir == NULL)
        errExit("opendir");

    /* Read all entries */
    char fdDirectoryPath[PATH_MAX];
    char fdSymlinkPath[PATH_MAX];
    char fdSymlinkTargetPath[PATH_MAX];
    errno = 0;
    for (struct dirent *direntp = readdir(procdir); direntp != NULL; direntp = readdir(procdir)) {
        /* Skip non-PID directories */
        if (!isdigit(*direntp->d_name))
            continue;

        /* Open the fd directory corresponding process with this PID */
        snprintf(fdDirectoryPath, PATH_MAX, "/proc/%s/fd", direntp->d_name);
        DIR *fddir = opendir(fdDirectoryPath);
        if (fddir == NULL)
            errExit("opendir");

        for (struct dirent *fddirentp = readdir(fddir); fddirentp != NULL; fddirentp = readdir(fddir)) {
            /* Skip files that are not PID symlinks */
            if (!isdigit(*fddirentp->d_name))
                continue;

            snprintf(fdSymlinkPath, PATH_MAX, "%s/%s", fdDirectoryPath, fddirentp->d_name);
            /* does not put terminator null byte */
            ssize_t len = readlink(fdSymlinkPath, fdSymlinkTargetPath, PATH_MAX - 1); 
            if (len == -1)
                errExit("readlink");
            /* place null terminator not placed by readlink */
            fdSymlinkTargetPath[len] = '\0';

            if (strcmp(fdSymlinkTargetPath, path) == 0) {
                puts(direntp->d_name);
                break;
            }
        }

        /* Potential readdir error */
        if (errno != 0)
            errExit("readdir"); 
        if (closedir(fddir) == -1)
            errExit("closedir");
    }
    /* Potential readdir error */
    if (errno != 0)
        errExit("readdir"); /* Potential readdir error */
    if (closedir(procdir) == -1)
        errExit("closedir");

    exit(EXIT_SUCCESS);
}

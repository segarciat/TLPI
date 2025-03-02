#include "tlpi_hdr.h"
#include "ugid_functions.h"
#include <sys/types.h>      // uid_t
#include <dirent.h>         // opendir, readdir, struct dirent, DIR
#include <unistd.h>         // pathconf
#include <string.h>         // strlen, strncmp, strncpy
#include <ctype.h>          // isdigit, isspace
#include <stdio.h>          // fopen, fgets, printf, snprintf, FILE
#include <stdbool.h>        // bool

#define UID_LEN_MAX 100
#define LINE_MAX 4096
#define CMD_MAX LINE_MAX
#define CMD_NAME_FIELD "Name:"
#define UID_FIELD "Uid:"

/*
 * Given the file pointer for a /proc/PID/status file, parses the file
 * to determine the name of the command that was used to execute this process
 * and the UID of the user that executed the command. If the UID
 * matches uid_str, then it prints the command and the process PID.
 */
void printIfOwnProcess(FILE *fp, uid_t uid, const char *pid_str)
{
    char line[LINE_MAX];
    char command[CMD_MAX + 1];
    const size_t cmdNameFieldSize = strlen(CMD_NAME_FIELD);
    const size_t uidFieldSize = strlen(UID_FIELD);
    bool uidMatches = false;
    command[0] = '\0';

    // Search for UID and (command) Name fields
    while (fgets(line, LINE_MAX, fp) != NULL) {
        if (!uidMatches && strncmp(line, UID_FIELD, uidFieldSize) == 0) {
            // Verify the uid matches.
            char *endp;
            long procUid = strtol(line + uidFieldSize, &endp, 0);
            uidMatches = ((uid_t) procUid == uid);
            if (!uidMatches)
                break;
        } else if (*command == '\0' && strncmp(line, CMD_NAME_FIELD, cmdNameFieldSize) == 0) {
            // Store command name.
            size_t offset = cmdNameFieldSize;
            while (isspace(line[offset]))
                offset++;
            strncpy(command, line + offset, CMD_MAX);
        }
    }
    if (*command != '\0' && uidMatches)
        printf("PID: %s\t\tCommand: %s", pid_str, command);
}

/*
 * pslist.c
 *
 * Usage: ./pslist user
 *
 * Lists all processes by run by the specified user.
 */

int
main(int argc, char *argv[])
{
    /* Obtain user id */
    if (argc < 2 || argc > 2 || strcmp(argv[1], "--help") == 0) {
        usageErr(   "%s user\n"
                    "Lists all processes being run by the specified user\n",
                    argv[0]);
    }
    uid_t uid = userIdFromName(argv[1]);
    if (uid == (uid_t) -1)
        fatal("Failed to get uid for %s", argv[1]);
    
    long maxPathname = pathconf("/proc", _PC_NAME_MAX);
    if (maxPathname == -1)
        maxPathname = 4096; /* Guess max pathname */
    char pidFilePath[maxPathname];

    /* Open /proc file-system */
    DIR *procdirp = opendir("/proc");
    if (procdirp == NULL)
        errExit("opendir");

    /* Find process matching UID */
    for (struct dirent *entp = readdir(procdirp); entp != NULL; entp = readdir(procdirp)) {
        /* Skip non-PID directories */
        if (!isdigit(*entp->d_name))
            continue;

        /* Build the file path */
        if (snprintf(pidFilePath, (size_t) maxPathname, "/proc/%s/status", entp->d_name) < 0)
            errExit("snprintf");

        /* Handle the possibility that the process may no longer exist */
        FILE* fp = fopen(pidFilePath, "r");
        if (fp == NULL) {
            if (errno == ENOENT)
                continue;
            else
                errExit("fopen");
        }
        printIfOwnProcess(fp, uid, entp->d_name);
        fclose(fp);
    }
    if (closedir(procdirp) == -1)
        errExit("closedir");
    exit(EXIT_SUCCESS);
}

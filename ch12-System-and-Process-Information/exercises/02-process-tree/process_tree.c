#include <sys/types.h>      /* pid_t */
#include <dirent.h>         /* DIR, opendir, readdir */
#include <linux/limits.h>   /* PATH_MAX */
#include <stddef.h>         /* NULL */
#include <stdio.h>
#include <ctype.h>          /* isdigit */
#include <errno.h>
#include "bst.h"
#include "tlpi_hdr.h"

#define LINE_MAX 4096
#define CMD_NAME_FIELD "Name:"
#define PPID_FIELD "PPid:"

struct pinfo_type {
    pid_t pid;
    pid_t ppid;
    char *cmd;
    struct pinfo_type *children;
    struct pinfo_type *next;
};

/*
 * Creates a pinfo_type object.
 */
struct pinfo_type *pinfo_type_create(char *cmd, pid_t pid, pid_t ppid)
{
    struct pinfo_type *pinfo = malloc(sizeof(struct pinfo_type));
    if (pinfo == NULL)
        return NULL;
    pinfo->pid = pid;
    pinfo->ppid = ppid;
    pinfo->cmd = cmd;
    pinfo->next = NULL;
    pinfo->children = NULL;
    return pinfo;
}

/*
 * Comparison function for bst procedures. Compares pinfo_type objects by pid
 */
int cmpPid(const void *p, const void *q)
{
    const pid_t *pPid = p;
    const pid_t *qPid = q;

    if (*pPid < *qPid)
        return -1;
    else if (*pPid > *qPid)
        return 1;
    return 0;
}

/*
 * Given a file pointer fp pointing to a pid/PID/status file,
 * searches for the commd name and parent process ID.
 * Sets cmd to point to a dynamically alloated buffer with
 * the name, and updates ppid to have the parent process id.
 */
void parsePidInfo(FILE *fp, char **cmd, pid_t *ppid)
{
    char line[LINE_MAX];
    const size_t cmdNameFieldLen = strlen(CMD_NAME_FIELD);
    const size_t ppidFieldLen = strlen(PPID_FIELD);
    bool ppidFound = false;
    *cmd = NULL;
    while ((!ppidFound || *cmd == NULL) && fgets(line, sizeof(line), fp) != NULL) {
        // Parse command name
        if (*cmd == NULL && strncmp(line, CMD_NAME_FIELD, cmdNameFieldLen) == 0) {
            /* Skip spaces */
            size_t offset = cmdNameFieldLen;
            while (isspace(line[offset]))
                offset++;

            /* Overwrite \n character */ 
            size_t cmdSize = strlen(line + offset);
            line[offset + cmdSize - 1] = '\0';
            *cmd = malloc(cmdSize + 1);
            if (*cmd == NULL)
                errExit("malloc");
            strcpy(*cmd, line + offset);
        } else if (!ppidFound && strncmp(line, PPID_FIELD, ppidFieldLen) == 0) {
            size_t offset = cmdNameFieldLen;
            while (isspace(line[offset]))
                offset++;
            char *endp;
            errno = 0;
            *ppid = (pid_t) strtol(line + offset, &endp, 0);
            if (errno != 0)
                errExit("strtol");
            ppidFound = true;
        }
    }
}

/*
 * Given a key-value pair corresponding to a process,
 * finds the process' parent which is stored in the BST t.
 * Updates the children list of the parent to contain this
 * process.
 */
void populateChildren(BST t, const void *key, void *val)
{
    const pid_t *pid = key;
    struct pinfo_type *pinfo = val;

    // Skip init process
    if (*pid == pinfo->ppid) {
        return;
    }

    // Find parent
    struct pinfo_type *parent = bst_get(t, &pinfo->ppid, cmpPid);

    // Add this process to head of parent's list of children
    pinfo->next = parent->children;
    parent->children = pinfo;
}


/*
 * Recursively displays the parent-child hierarchy of process
 * implied by the pinfo object given. The number of spaces
 * should be given as 0.
 */
void printProcessTree(struct pinfo_type *pinfo, size_t spaces)
{
    if (pinfo == NULL) {
        return;
    }

    // print self
    spaces += (size_t) printf("%s(%d)", pinfo->cmd, pinfo->pid);
    // print children
    for (struct pinfo_type *child = pinfo->children; child != NULL; child = child->next) {
        printf("->");
        printProcessTree(child, spaces + 2);
        if (child->next != NULL) {
            printf("\n");
            for (size_t i = 0; i < spaces; i++)
                putchar(' ');
        }
    }
}

int main()
{
    // Create tree of processes
    BST ptree = bst_create();
    if (ptree == NULL)
        errExit("bst_create");

    // Add init process to tree
    char *initProcessName = malloc(sizeof("init"));
    if (initProcessName == NULL)
        errExit("malloc");
    strcpy(initProcessName, "init");
    struct pinfo_type *initProcess = pinfo_type_create(initProcessName, 0, 0);
    if (initProcess == NULL)
        errExit("pinfo_type_create");
    bst_put(ptree, &initProcess->pid, initProcess, cmpPid);

    // Read proc PID entries
    DIR *procdir = opendir("/proc");
    if (procdir == NULL)
        errExit("opendir");
    errno = 0;
    for (struct dirent *direntp = readdir(procdir); direntp != NULL; direntp = readdir(procdir)) {
        /* Skip non-PID directory */
        if (!isdigit(*direntp->d_name))
            continue;

        char pidFileName[PATH_MAX];
        pid_t pid = (pid_t) getLong(direntp->d_name, GN_GT_0, "pid");
        snprintf(pidFileName, PATH_MAX, "/proc/%s/status", direntp->d_name);

        /* Handle the possibility that process may no longer exist */
        FILE *fp = fopen(pidFileName, "r");
        if (fp == NULL) {
            if (errno = ENOENT) {
                errno = 0;
                continue;
            }
            else {
                errExit("fopen");
            }
        }
        /* Store process info node */
        char *cmd;
        pid_t ppid;
        parsePidInfo(fp, &cmd, &ppid);
        struct pinfo_type *pinfo = pinfo_type_create(cmd, pid, ppid);
        if (pinfo == NULL)
            errExit("malloc");
        bst_put(ptree, &pinfo->pid, pinfo, cmpPid);
    }
    if (closedir(procdir) == -1)
        errExit("closedir");

    /* Set up parent-children relationships */
    bst_walk(ptree, populateChildren);

    /* Print process tree */
    printProcessTree(initProcess, 0);
    printf("\n");

    exit(EXIT_SUCCESS);
}

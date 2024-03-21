#include "tlpi_hdr.h" /* Error-handling functions */
#include <dirent.h> /* DIR, opendir, readdir */
#include <errno.h> /* errno */
#include <stdio.h> /* puts, FILE, fopen, BUFSIZ */
#include <ctype.h> /* isdigit */
#include <sys/types.h> /* pid_t */
#include <stdbool.h> /* true */
#include "process.h"

#define PROCESS_NAME_HEADER "Name:"
#define PPID_NAME_HEADER "PPid:"

void
setCommand(char *command, char *line)
{
	while (isspace(*line)) /* Skip white space */
		line++;
	strncpy(command, line, BUFSIZ);
	size_t cmdLen = strlen(command);
	if (command[cmdLen - 1] == '\n')
		command[cmdLen - 1] = '\0';
}

pid_t
getPPid(const char *line)
{
	while (isspace(*line))
		line++;
	char *endp;
	pid_t ppid = (pid_t) strtol(line, &endp, 10); /* Base 10 */
	if (*endp != '\0' && !isspace(*endp))
		fatal("strol");
	return ppid;
}

void
addProcChildren(struct pnode *root, struct pnode *node)
{
	if (node != NULL) {
		addProcChildren(root, node->left);
		if (node->proc->pid != node->proc->ppid) {
			struct pnode *parent = pnode_find(root, node->proc->ppid);
			process_insert_child(parent->proc, node->proc);
		}
		addProcChildren(root, node->right);
	}
}

void
printProcessTree(struct process *proc, size_t spaces)
{
	if (proc != NULL) {
		spaces += printf("%s(%d)", proc->cmd, proc->pid);
		for (struct proclist *pl = proc->firstChild; pl != NULL; pl = pl->next) {
			printf("->");
			printProcessTree(pl->proc, spaces + 2); // two spaces for: ->
			if (pl->next != NULL) {
				printf("\n");
				for (size_t i = 0; i < spaces; i++) // Add proper indent
					putchar(' ');
			}
		}
	}
}

int
main(int argc, char *argv[])
{
	/* Open /proc directory */
	DIR *procdir = opendir("/proc");
	if (procdir == NULL)
		errExit("opendir");
	/* Create node for init process with no parent */
	struct pnode *root = pnode_new(process_new(0, "init", 0)); /* init is its own parent, so we will say it has PPid 0 */
	if (root == NULL)
		errExit("malloc");

	/* Read all entries in the directory */
	errno = 0;
	char pidStatusFileName[BUFSIZ];
	char line[BUFSIZ];
	char command[BUFSIZ];
	const size_t commandHeaderLen = strlen(PROCESS_NAME_HEADER);
	const size_t ppidHeaderLen = strlen(PPID_NAME_HEADER);
	for (struct dirent *direntp = readdir(procdir); direntp != NULL; errno = 0, direntp = readdir(procdir)) {
		if (!isdigit(*direntp->d_name))
			continue; /* Skip if it does not start with a digit, i.e., if it is not a PID directory */
		pid_t pid = (pid_t) getLong(direntp->d_name, GN_GT_0, "pid");
		if (snprintf(pidStatusFileName, BUFSIZ, "/proc/%s/status", direntp->d_name) < 0)
			errExit("snprintf");

		/* Open the status file for current process */
		// pid_t pid = getLong(direntp->d_name, GN_GT_0, direntp->d_name);
		FILE *fp = fopen(pidStatusFileName, "r");
		if (fp == NULL)
			errExit("fopen");
		/* Read the startup command name and PPID  */
		pid_t ppid = -1;
		*command = '\0';
		while (fgets(line, BUFSIZ, fp) != NULL) {
			if (*command == '\0' && strncmp(line, PROCESS_NAME_HEADER, commandHeaderLen) == 0) {
				setCommand(command, line + commandHeaderLen);
			} else if (strncmp(line, PPID_NAME_HEADER, ppidHeaderLen) == 0) {
				ppid = getPPid(line + ppidHeaderLen);
				break;
			}
		}
		if (errno != 0)
			errExit("fgets");
		if (fclose(fp) != 0)
			errExit("fclose");
		root = pnode_insert(root, process_new(pid, command, ppid));
		if (root == NULL)
			fatal("pnode_insert");
	}
	/* Handle potential readdir error */
	if (errno != 0) {
		errExit("readdir");
	}
	if (closedir(procdir) == -1)
		errExit("closeDir");
	addProcChildren(root, root);
	printProcessTree(root->proc, 0);
	printf("\n");


	exit(EXIT_SUCCESS);
}

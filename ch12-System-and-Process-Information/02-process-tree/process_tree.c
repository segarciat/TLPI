#include "tlpi_hdr.h" /* Error-handling functions */
#include <dirent.h> /* DIR, opendir, readdir */
#include <errno.h> /* errno */
#include <stdio.h> /* puts, FILE, fopen, BUFSIZ */
#include <ctype.h> /* isdigit */
#include <sys/types.h> /* pid_t */
#include <stdbool.h> /* true */

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

int
main(int argc, char *argv[])
{
	/* Open /proc directory */
	DIR *procdir = opendir("/proc");
	if (procdir == NULL)
		errExit("opendir");
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
		printf("%s (%s) with PPid: %ld\n", command, direntp->d_name, (long) ppid);
	}
	/* Handle potential readdir error */
	if (errno != 0) {
		errExit("readdir");
	}
	if (closedir(procdir) == -1)
		errExit("closeDir");

	exit(EXIT_SUCCESS);
}

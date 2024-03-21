#include "tlpi_hdr.h"
#include <string.h> // strcmp
#include <linux/limits.h> // PATH_MAX
#include <stdlib.h> // realpath
#include <dirent.h> // DIR, opendir, readdir
#include <errno.h> // errno
#include <ctype.h> // isdigit
#include <sys/types.h> // pid_t

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
	errno = 0;
	char fdDirectoryPath[PATH_MAX];
	char fdSymlinkPath[PATH_MAX];
	char fdSymlinkTargetPath[PATH_MAX];
	for (struct dirent *direntp = readdir(procdir); direntp != NULL; direntp = readdir(procdir)) {
		if (!isdigit(*direntp->d_name)) /* Skip non-processes */
			continue;
		/* Open the fd directory */
		// pid_t pid = (pid_t) getLong(direntp->d_name, GN_GT_0, "pid");
		if (snprintf(fdDirectoryPath, PATH_MAX, "/proc/%s/fd", direntp->d_name) < 0)
			errExit("snprintf");
		DIR *fddir = opendir(fdDirectoryPath);
		if (fddir == NULL)
			errExit("opendir");
		for (struct dirent *fddirentp = readdir(fddir); fddirentp != NULL; fddirentp = readdir(fddir)) {
			if (!isdigit(*fddirentp->d_name))
				continue;
			if (snprintf(fdSymlinkPath, PATH_MAX, "%s/%s", fdDirectoryPath, fddirentp->d_name) < 0)
				errExit("snprintf");
			ssize_t len = readlink(fdSymlinkPath, fdSymlinkTargetPath, PATH_MAX); /* does not put terminator null byte */
			if (len == -1)
				errExit("readlink");
			if (len != PATH_MAX)
				fdSymlinkTargetPath[len] = '\0'; /* place null terminator not placed by readlink */
			if (strncmp(fdSymlinkTargetPath, path, PATH_MAX) == 0) {
				puts(direntp->d_name);
				break;
			}
		}
		if (errno != 0)
			errExit("readdir"); /* Potential readdir error */
		if (closedir(fddir) == -1)
			errExit("closedir");
	}
	if (errno != 0)
		errExit("readdir"); /* Potential readdir error */
	if (closedir(procdir) == -1)
		errExit("closedir");
	exit(EXIT_SUCCESS);
}

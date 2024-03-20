#include "tlpi_hdr.h"
#include <sys/types.h> // uid_t
#include <pwd.h> // getpwnam, struct passwd
#include <dirent.h> // opendir, readdir, struct dirent, DIR
#include <unistd.h> // sysconf
#include <string.h> // strlen, strncmp, strstr, strncpy
#include <ctype.h> // isdigit, isspace
#include <stdio.h> // fopen, fgets, printf, snprintf, FILE
#include <stdbool.h> // bool

#define CMD_MAX 1000
#define UID_LEN_MAX 100
#define MAXLN 1000

uid_t
getUidFromUser(char *user)
{
	if (user == NULL || *user == '\0')
		return -1;
	struct passwd *userpw = getpwnam(user);
	if (userpw == NULL)
		return -1;
	return userpw->pw_uid;
}

int
main(int argc, char *argv[])
{
	/* Read provided user */
	if (argc < 2 || argc > 2 || *argv[1] == '\0' || strcmp(argv[1], "--help") == 0)
		usageErr("%s user\n", argv[0]);
	long loginNameMax = sysconf(_SC_LOGIN_NAME_MAX);
	if (loginNameMax == -1)
		loginNameMax = 256; /* Guess max login name length */
	if (strlen(argv[1]) > loginNameMax)
		cmdLineErr("%s exceeds math length of %ld\n", loginNameMax);
	
	/* Find and save matching UID */
	uid_t uid = getUidFromUser(argv[1]);
	if (uid == -1)
		fatal("Could not find record for %s\n", argv[1]);
	char uid_str[UID_LEN_MAX];
	if (snprintf(uid_str, UID_LEN_MAX, "%ld", (long) uid) < 0)
		errExit("snprintf");
	
	/* Open /proc file-system */
	DIR *procdirp = opendir("/proc");
	if (procdirp == NULL)
		errExit("opendir");
	long maxPathname = pathconf("/proc", _PC_NAME_MAX);
	if (maxPathname == -1)
		maxPathname = 4096; /* Guess max pathname */
	char pidStatusPath[maxPathname], line[MAXLN], command [CMD_MAX];

	/* Find process matching UID */
	for (struct dirent *entp = readdir(procdirp); entp != NULL; entp = readdir(procdirp)) {
		/* Skip self, parent, and non-PID directories */
		if (!isdigit(*entp->d_name) || strcmp(entp->d_name, ".") == 0 || strcmp(entp->d_name, "..") == 0)
			continue;

		if (snprintf(pidStatusPath, maxPathname, "/proc/%s/status", entp->d_name) < 0)
			errExit("snprintf");
		FILE* fp = fopen(pidStatusPath, "r");
		if (fp == NULL) {
			if (errno == ENOENT)
				continue; /* Process no longer exists */
			else
				errExit("fopen");
		}
		*command = '\0';
		bool match = false;		
		while (fgets(line, MAXLN, fp) != NULL) {
			if (!match && strncmp(line, "Name:", 5) == 0) {
				size_t offset = 5;
				while (isspace(line[offset]))
					offset++;
				strncpy(command, line + offset, CMD_MAX - 1);
			}
			else if (strstr(line, "Uid:") && strstr(line, uid_str)) {
				match = true;
				break;
			}
		}
		fclose(fp);
		if (*command != '\0' && match)
			printf("PID: %s\t\tCommand: %s", entp->d_name, command);
	}
	if (closedir(procdirp) == -1)
		errExit("closedir");
	exit(EXIT_SUCCESS);
}

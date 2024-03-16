#include "initgroups.h"
#define _BSD_SOURCE
#include <grp.h> // setgroups
#include <pwd.h> // getpwnam, getgrent, endgrent, struct group
#include <stdlib.h> // malloc
#include <unistd.h> // sysconf
#include <string.h> // strcmp

/* Change the supplementary IDs of the process to those in /etc/group plus the provided group
 * Returns 0 on success, -1 on error
 */
int
initgroups_s(const char *user, gid_t group)
{
	/* Ensure a valid user and group were provided */
	if (user == NULL || *user == '\0' || getpwnam(user) == NULL || getgrgid(group) == NULL)
		return -1;

	/* Open /etc/group file and restart from beginning */
	struct group *gp = getgrent();
	if (gp == NULL)
		return -1;

	/* Allocate space for list of groups the user may belong to */
	size_t maxGroupSetLen = sysconf(_SC_NGROUPS_MAX) + 1;
	gid_t *groupSet = malloc(maxGroupSetLen);
	if (groupSet == NULL) {
		endgrent(); // Close group file.
		return -1;
	}

	/* Add given group to the set */
	size_t groupSetLen = 0;
	groupSet[groupSetLen++] = group;

	/* Populate set with groups the user is a member of */
	setgrent();
	for (; gp != NULL; gp = getgrent()) {
		if (gp->gr_gid == group) // Do not re-add `group` to set.
			continue;
		for (char **member = gp->gr_mem; member != NULL; member++)
			if (strcmp(*member, user) == 0)
				groupSet[groupSetLen++] = gp->gr_gid;
	}
	/* Close the /etc/group file */
	endgrent();

	/* Attempt to replace the process's supplementary groups with built set
	 * (process must be privileged for this system call to succeed
	 */
	int status = setgroups(groupSetLen, groupSet);
	free(groupSet);
	return status;
}

#include <sys/stat.h>
#include <unistd.h>	/* F_OK, R_OK, W_OK, X_OK, geteuid, getegid, getgroups, sysconf */


/* Checks accessibility of the file specified in pathname based on
 * the process' effective used ID and group.
 * 
 * Returns 0 if the process has the requested access, and -1 otherwise
 */
int
eaccess(const char *pathname, int mode)
{
	/* Get the file's i-node */
	struct stat sb;
	if (stat(pathname, &sb) == -1)
		return -1;

	/* Client only interested in the file existing */
	if ((mode & F_OK) && !(mode & (R_OK | W_OK | X_OK)))
		return 0;
	
	/* Get the process's EUID */
	uid_t euid = geteuid();
	gid_t egid = getegid();

	/* Root is granted all access */
	if (euid == 0 || egid == 0)
		return 0;

	int checkRead = mode & R_OK;
	int checkWrite = mode & W_OK;
	int checkExecute = mode & X_OK;

	/* Check if user has access */
	if (euid == sb.st_uid &&
		(!checkRead || (checkRead && (sb.st_mode & S_IRUSR))) &&
		(!checkWrite || (checkWrite && (sb.st_mode & S_IWUSR))) &&
		(!checkExecute || (checkExecute && (sb.st_mode & S_IXUSR))))
		return 0;

	/* Get supplementary groups for the process */
	int MAX_GID_SET_SIZE = sysconf(_SC_NGROUPS_MAX) + 1;
	if (MAX_GID_SET_SIZE == -1)
		MAX_GID_SET_SIZE = 65536; /* Make a guess */
	gid_t grouplist[MAX_GID_SET_SIZE];
	int numgroups = getgroups(MAX_GID_SET_SIZE, grouplist);
	if (numgroups == -1)
		return -1;
	
	/* Add egid in case not present */
	grouplist[numgroups++] = egid;

	/* Check supplementary list for root or the file's group ID */
	for (int i = 0; i < numgroups; i++) {
		/* Root group in supplementary list */
		if (grouplist[i] == 0)
			return 0;

		/* Permissions according to supplementary group matching file group */
		if (grouplist[i] == sb.st_uid) {
			if ((!checkRead || (checkRead && (sb.st_mode & S_IRGRP))) &&
				(!checkWrite || (checkWrite && (sb.st_mode & S_IWGRP))) &&
				(!checkExecute || (checkExecute && (sb.st_mode & S_IXGRP))))
				return 0;
			break;
		}
	}
	
	/* Permissions according to 'other' */
	if ((!checkRead || (checkRead && (sb.st_mode & S_IROTH))) &&
		(!checkWrite || (checkWrite && (sb.st_mode & S_IWOTH))) &&
		(!checkExecute || (checkExecute && (sb.st_mode & S_IXOTH))))
		return 0;

	/* Insufficient access */
	return -1;
}

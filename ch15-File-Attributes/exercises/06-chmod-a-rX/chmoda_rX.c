#include "tlpi_hdr.h"
#include <sys/stat.h> // stat, struct stat

/* Equivalent to "chmod a+rX file". Enables read permission for all categories of user,
 * and likewise, enables execute permissions for all categories of user if file
 * is a directory or execute permission is enabled for any of the user categories
 * of file.
 */
int
main(int argc, char *argv[])
{
	if (argc != 2)
		usageErr("%s file\n", argv[0]);

	/* Get the file's i-node */
	struct stat sb;
	if (stat(argv[1], &sb) == -1)
		errExit("stat");

	/* Enable all read permissons */
	mode_t updatedPerms = sb.st_mode | S_IRUSR | S_IRGRP | S_IROTH;

	/* Enable all execs if directory or if any user has exec perms */
	mode_t allExecs = S_IXUSR | S_IXGRP | S_IXOTH;
	if ((sb.st_mode & allExecs) || S_ISDIR(sb.st_mode)) {
		updatedPerms |= allExecs;
	}

	if (chmod(argv[1], updatedPerms) == -1)
		errExit("chmod");
	return EXIT_SUCCESS;
}

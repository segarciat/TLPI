#include "tlpi_hdr.h"
#include <string.h> /* strcmp, strlen */
#include <sys/xattr.h>	/* setxattr, getxattr */
#include <linux/limits.h> /* XATTR_NAME_MAX, XATTR_SIZE_MAX */

/* CLI args */
#define FILE_ARG_INDX 1
#define EANAME_ARG_INDX 2
#define EAVAL_ARG_INDX 3

#define USER_NS_PREFIX "user."

/* Create or modify a user EA for a file, given the name and value */
int
main(int argc, char *argv[])
{
	if (argc != 4 || strcmp(argv[1], "--help") == 0)
		usageErr("%s file ea_name ea_val\n", argv[0]);

	/* Validate arguments */
	if (strnlen(argv[EANAME_ARG_INDX], XATTR_NAME_MAX + 1) > XATTR_NAME_MAX)
		cmdLineErr("Extended attribute name length exceeds %d\n", XATTR_NAME_MAX);
	
	size_t valSize = strnlen(argv[EAVAL_ARG_INDX], XATTR_SIZE_MAX + 1);
	if (valSize > XATTR_SIZE_MAX)
		cmdLineErr("Extended attribute value length exceeds %d\n", XATTR_SIZE_MAX);
	

	/* Add user namespace prefix */
	char eanameBuf[strlen(USER_NS_PREFIX) + XATTR_NAME_MAX + 1];
	if (snprintf(eanameBuf, XATTR_NAME_MAX + 1, "%s%s", USER_NS_PREFIX, argv[EANAME_ARG_INDX]) < 0)
		fatal("snprintf");
	
	if (setxattr(argv[FILE_ARG_INDX], eanameBuf, argv[EAVAL_ARG_INDX], valSize, 0) == -1)
		errExit("setxattr");
	return EXIT_SUCCESS;
}

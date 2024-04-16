#include "tlpi_hdr.h"
#include <linux/fs.h>	/* FS_IOC_GETFLAGS, FS_IOC_SETFLAGS */
#include <sys/ioctl.h>	/* ioctl system call */
#include <fcntl.h>		/* open, close */
#include <string.h>		/* strncmp, strlen */

#define VALID_FIRST_PARAM "[-+=acDijAdtsSTu]"

int
main(int argc, char *argv[])
{
	if (argc < 3 || strncmp(argv[1], "--help", strlen("--help")) == 0)
		usageErr("%s %s files...\n", VALID_FIRST_PARAM, argv[0]);

	/* Validate action */
	char action = *argv[1];
	if (action != '=' && action != '+' && action != '-')
		cmdLineErr("Must use =, +, or -\n");

	/* Build flags */
	int parsedFlags = 0;
	for (char *p = argv[1] + 1; *p != '\0'; p++) {
		switch(*p) {
			case 'a': parsedFlags |= FS_APPEND_FL		; break;
			case 'c': parsedFlags |= FS_COMPR_FL		; break;
			case 'D': parsedFlags |= FS_DIRSYNC_FL		; break;
			case 'i': parsedFlags |= FS_IMMUTABLE_FL	; break;
			case 'j': parsedFlags |= FS_JOURNAL_DATA_FL	; break;
			case 'A': parsedFlags |= FS_NOATIME_FL		; break;
			case 'd': parsedFlags |= FS_NODUMP_FL		; break;
			case 't': parsedFlags |= FS_NOTAIL_FL		; break;
			case 's': parsedFlags |= FS_SECRM_FL		; break;
			case 'S': parsedFlags |= FS_SYNC_FL			; break;
			case 'T': parsedFlags |= FS_TOPDIR_FL		; break;
			case 'u': parsedFlags |= FS_UNRM_FL			; break;
			default:
				cmdLineErr("Invalid attribute %c in argument %s\n", *p, argv[1]);
				break;
		}
	}
	if (parsedFlags == 0)
		cmdLineErr("Missing attributes, valid format: %s\n", VALID_FIRST_PARAM);

	/* Apply chattr to all given files */
	for (int i = 2; i < argc; i++) {
		/* Open file for reading */
		int fd = open(argv[i], O_RDONLY);
		if (fd == -1)
			errExit("open %s\n", argv[i]);

		/* Set, add, or remove attributes */
		int updatedAttr = 0;
		if (action == '=') {
			updatedAttr = parsedFlags;
		} else {
			int oldAttr;
			if (ioctl(fd, FS_IOC_GETFLAGS, &oldAttr) == -1)
				errExit("ioctl %s\n", argv[i]);
			switch(action) {
				case '+': updatedAttr = oldAttr |   parsedFlags ; break;
				case '-': updatedAttr = oldAttr & (~parsedFlags); break;
				default : break;
			}
		}
		
		/* Update parsedFlags */
		if (ioctl(fd, FS_IOC_SETFLAGS, &updatedAttr) == -1)
			errExit("ioctl %s\n", argv[i]);

		/* Close file */
		if (close(fd) == -1)
			errExit("close %s\n", argv[i]);
	}
	return EXIT_SUCCESS;
}

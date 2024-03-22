#define _FILE_OFFSET_BITS 64 // support large file IO
#include "tlpi_hdr.h"
#include <unistd.h> // getopt, lseek, read, write
#include <fcntl.h> // open
#include <sys/types.h> // off_t

#ifndef BUF_SIZ
#define BUF_SIZ 4096
#endif

#define DEFAULT_LINES 10
#define USAGE_FORMAT "%s [-n num] file\n"

int
main(int argc, char *argv[])
{
	/* Read possible value of -n */
	off_t n = DEFAULT_LINES;
	int opt;
	while ((opt = getopt(argc, argv, "n:")) != -1) {
		switch(opt) {
			case 'n':
				n = (off_t) getLong(optarg, GN_ANY_BASE | GN_GT_0, "n");
				break;
			default:
				usageErr(USAGE_FORMAT, argv[0]);
				break;
		}
	}
	/* Ensure a single file was specified */
	if (optind != argc - 1)
		usageErr(USAGE_FORMAT, argv[0]);
	
	int fd = open(argv[optind], O_RDONLY);
	if (fd == -1)
		errExit("open");
	/* Determine the number of bytes in the file */
	off_t offset;
	if ((offset = lseek(fd, 0, SEEK_END)) == -1)
		errExit("lseek");

	/* Look for newline characters starting BUF_SIZ from the end of the file */
	ssize_t numRead;
	char buf[BUF_SIZ];
	while (offset > 0 && n >= 0) {
		if (offset <= BUF_SIZ)
			offset = 0;
		else
			offset -= BUF_SIZ;
		/* Move file pointer back to start OR back by BUF_SIZ */
		if (lseek(fd, offset, SEEK_SET) == -1)
			errExit("lseek");

		/* Read up to BUF_SIZ bytes */
		if ((numRead = read(fd, buf, BUF_SIZ)) == 0)
			break;

		/* Look for newline characters */
		for (ssize_t i = numRead - 1; i >= 0; i--) {
			/* Save offset of n-th line from the end of file */
			if (buf[i] == '\n' && --n < 0) {
				offset += i + 1;
				break;
			}
		}
	}
	if (numRead == -1)
		errExit("read");

	/* Move to n-th line from the end and output all lines */
	if (lseek(fd, offset, SEEK_SET) == -1)
		errExit("lseek");
	while ((numRead = read(fd, buf, BUF_SIZ)) > 0)
		if (write(STDOUT_FILENO, buf, numRead) != numRead)
			fatal("write() error or partial write");
	if (numRead == -1)
		errExit("read");

	if (close(fd) == -1)
		errExit("close");
	exit(EXIT_SUCCESS);
}

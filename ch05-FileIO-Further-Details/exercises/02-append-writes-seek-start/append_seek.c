#include <stdio.h>    // fprintf, stderr
#include <stdlib.h>   // exit, EXIT_FAILURE, EXIT_SUCCESS
#include <string.h>   // strcmp
#include <sys/stat.h>
#include <fcntl.h>    // open
#include <unistd.h>   // lseek
#include <string.h>   // strlen, strcmp
#include "tlpi_hdr.h"


/*
	TLPI Exercise 05-02: 
	Opens a file in append mode, seeks to the beginning, and writes some content.
*/

int
main(int argc, char* argv[])
{
	if (argc != 2 || strcmp(argv[1], "--help") == 0)
		usageErr("%s existing_file\n", argv[0]);

	int openFlags = O_RDWR | O_APPEND;
	int fd = open(argv[1], openFlags, S_IRUSR | S_IWUSR);
	if (fd == -1)
		errExit("open");
	if (lseek(fd, 0, SEEK_SET) == -1)
		errExit("lseek");
	const char* text = "\nfirst\nsecond\n";
	if (write(fd, "\nfirst\nsecond\n", strlen(text)) == -1)
		errExit("write");
	exit(EXIT_SUCCESS);
}

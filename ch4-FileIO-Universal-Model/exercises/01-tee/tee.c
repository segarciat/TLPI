#include <stdlib.h>   // EXIT_FAILURE
#include <stdio.h>    // printf, fprintf, perror
#include <unistd.h>   // getopt, ssize_t
#include <sys/stat.h> // flags for open systemcall
#include <fcntl.h>    // open system call and its flags
#include <stdbool.h>  // bool, true, false

#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif

/* tee: read stdin and write to both stdout and a named file.

		By default, any content in the existing file is overwritten.
		If -a is specified, data is instead appended to the end of the file.
*/

static void
usageError(const char *progName)
{
	fprintf(stderr, "Usage: %s [-a] <filename>\n", progName);
	exit(EXIT_FAILURE);
}

static void
syscallError(const char* msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

int	
main(int argc, char *argv[])
{
	/* Read the optionally provided -a */
	bool append = false;
	int opt;
	while ((opt = getopt(argc, argv, "a")) != -1) {
		switch (opt) {
			case 'a':
				append = true;
				break;
			default:
				usageError(argv[0]);
				break;
		}
	}

	/* Enforce that only single filename argument is provided */
	if (optind != argc - 1)
		usageError(argv[0]);

	/* Open named file for writing, truncating content by default, and appending instead if -a is specified */
	int flags = O_WRONLY | O_CREAT;
	flags |= (append) ? O_APPEND : O_TRUNC;

	int fd = open(argv[optind], flags, S_IRUSR | S_IWUSR);
	if (fd == -1)
		syscallError("open");

	/* Read stdin */
	char buf[BUF_SIZE];
	ssize_t numRead;
	while ((numRead = read(STDIN_FILENO, buf, BUF_SIZE)) > 0) {
		/* Write to specified file */
		if (write(fd, buf, numRead) != numRead)
			syscallError("write file");
		/* Write to stdout */
		if(write(STDOUT_FILENO, buf, numRead) != numRead)
			syscallError("write stdout");
	}
	if (numRead == -1)
		syscallError("read stdin");
	if (close(fd) == -1)
		syscallError("close file");
	
	exit(EXIT_SUCCESS);
}



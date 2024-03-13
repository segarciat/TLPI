#include <stdio.h>  // fprintf, stderr, perror
#include <stdlib.h> // exit, EXIT_SUCCESS
#include <unistd.h> // read, write, lseek
#include <fcntl.h>  // open, open flags

#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif

/* copy: copy input file to output file, preserving holes */

static void
usageError(const char* progName)
{
	fprintf(stderr, "Usage: %s <old-file> <newfile>\n", progName);
	exit(EXIT_FAILURE);
}

static void
syscallError(const char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
	if (argc != 3)
		usageError(argv[0]);
	
	/* Open input and output files */
	int inputFd = open(argv[1], O_RDONLY);
	if (inputFd == -1)
		syscallError("opening input file");

	int openFlags = O_CREAT | O_WRONLY | O_TRUNC;
	int filePerms = S_IRUSR | S_IWUSR;
	int outputFd = open(argv[2], openFlags, filePerms);
	if (outputFd == -1)
		syscallError("opening output file");

	/* Transfer data until we encounter EOF or error */
	char buf[BUF_SIZE];
	ssize_t numRead;
	while ((numRead = read(inputFd, buf, BUF_SIZE)) >= 0)
		if (numRead == 0) {
			/* Determine if hole or if reached EOF */
			off_t curPos, eofPos;
			if ((curPos = lseek(inputFd, 0, SEEK_CUR)) == -1 || (eofPos = lseek(inputFd, 0, SEEK_END)) == -1)
				syscallError("lseek input file");
			if (curPos == eofPos)
				break;
			/* Seek past end to introduce hole in output file, and reset input file position */
			else if (lseek(outputFd, 1, SEEK_END) == -1 || lseek(inputFd, curPos, SEEK_SET) == -1)
				syscallError("lseek output file");
		} /* Write to output file */
		else if (write(outputFd, buf, numRead) != numRead)
			syscallError("write to output file");
	if (numRead == -1)
		syscallError("read error");

	if (close(inputFd) == -1)
		syscallError("close input file");
	if (close(outputFd) == -1)
		syscallError("close output file");
	
	exit(EXIT_SUCCESS);
}

#include <stdio.h>		/* FILE, stdout, stderr, fprintf() */
#include <stdlib.h>		/* exit(), EXIT_SUCCESS, EXIT_FAILURE */
#include <stdarg.h>		/* va_list, va_start(),va_end() */
#include <stdbool.h>	/* bool */
#include <errno.h>		/* errno */
#include <string.h>		/* strerror(), memcpy() */
#include <sys/types.h>
#include <unistd.h>		/* getopt(), ftruncate() */
#include <fcntl.h>		/* open() and flags */
#include <sys/stat.h>	/* fstat(), umask(), permission flags */
#include <sys/mman.h>	/* mmap(), msync(), and corresponding constants */

extern char *optarg;
extern int optind, opterr, optopt;


void
usage(FILE *stream, char *progName)
{
	fprintf(stream, "Usage: %s [-h] source-file destination-file\n", progName);
	fprintf(stream, "Copy source file to destination file. If the destination file exists, it is overwritten\n\n");
	fprintf(stream, "Options:\n");
	fprintf(stream, "    -h        View this help text.\n\n");
}

void
errorExit(bool showErrno, char *fmt, ...)
{
	va_list varArgp;

	va_start(varArgp, fmt);
	if (fmt != NULL)
		vfprintf(stderr, fmt, varArgp);
	if (showErrno) 
		fprintf(stderr, ": %s", strerror(errno));
	fprintf(stderr, "\n");
	va_end(varArgp);
	exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
	int opt, inputFd, outputFd;
	struct stat statBuf;
	char *inputMapAddr, *outputMapAddr;

	/* Parse command-line options, if-any */
	while ((opt =  getopt(argc, argv, "+h")) != -1) {
		switch (opt) {
			case 'h':
				usage(stdout, argv[0]);
				exit(EXIT_SUCCESS);
			case '?':
				usage(stderr, argv[0]);
				errorExit(false, "%s: Unrecognized option (-%c)", argv[0], optopt);
			default:
				errorExit(false, "%s: Unexpected case reached while parsing arguments", argv[0]);
		}
	}

	/* Enforce that both a source and a destination file path are provided */
	if (argc != optind + 2) {
		usage(stderr, argv[0]);
		errorExit(false, "%s: Incorrect number of arguments", argv[0]);
	}

	/* Open input file */
	inputFd = open(argv[optind], O_RDONLY);
	if (inputFd == -1)
		errorExit("%s: Failed to open %s", argv[0], argv[optind]);
	
	/* Obtain input file size */
	if (fstat(inputFd, &statBuf) == -1)
		errorExit(true, "%s: Failed to stat %s", argv[0], argv[optind]);
	
	/* Ensure we get the right permissions on output file */
	umask(0);

	/* Open output file with same permissions as input file */
	outputFd = open(argv[optind + 1], O_RDWR | O_CREAT,
		statBuf.st_mode & (
			S_IRUSR | S_IWUSR | S_IXUSR | 
			S_IRGRP | S_IWGRP | S_IXGRP |
			S_IROTH | S_IWOTH | S_IXOTH ));
	if (outputFd == -1)
		errorExit(true, "%s: Failed to create output file %s", argv[0], argv[optind + 1]);

	/* Set size of output file */
	if (ftruncate(outputFd, statBuf.st_size) == -1)
		errorExit(true, "%s: Failed to truncate output file", argv[0]);
	
	/* Create a private memory mapping for input file */
	inputMapAddr = mmap(NULL, statBuf.st_size, PROT_READ, MAP_PRIVATE, inputFd, 0);
	if (inputMapAddr == MAP_FAILED)
		errorExit(true, "%s: Failed to map input file", argv[0]);
	
	/* Create a private memory mapping for output file */
	outputMapAddr = mmap(NULL, statBuf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, outputFd, 0);
	if (outputMapAddr == MAP_FAILED)
		errorExit(true, "%s: Failed to map output file", argv[0]);
	
	/* Copy file */
	memcpy(outputMapAddr, inputMapAddr, statBuf.st_size);

	/* Synchronize file to disk */
	if (msync(outputMapAddr, statBuf.st_size, MS_SYNC) == -1)
		errorExit(true, "%s: msync() failed to synchronize output file to disk", argv[0]);

	exit(EXIT_SUCCESS);
}

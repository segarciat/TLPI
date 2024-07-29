#include <stdio.h>		/* fprintf(), stderr, stdout, FILE */
#include <stdlib.h>		/* exit(), EXIT_FAILURE, EXIT_SUCCESS */
#include <string.h>		/* strcmp(), strerror(), memset(), memcpy(), strlen() */
#include <stdbool.h>	/* bool, false, true */
#include <stdarg.h>		/* va_list va_start(), va_end() */
#include <errno.h>		/* errno() */
#include <fcntl.h>		/* open(), O_RDWR */
#include <sys/stat.h>	/* struct stat, fstat() */
#define _BSD_SOURCE		/* Expose madvise() in <sys/mman.h> */
#include <sys/mman.h>	/* mmap(), madvise() */

#define BYTES_READ 32

void
unix_error(bool showErrno, const char* fmt, ...)
{
	if (fmt != NULL) {
		va_list varArgp;
		va_start(varArgp, fmt);
		vfprintf(stderr, fmt, varArgp);
		va_end(varArgp);
	}
	if (showErrno)
		fprintf(stderr, "%s%s", (fmt != NULL) ? ": " : "", strerror(errno));
	fprintf(stderr, "\n");
	exit(EXIT_FAILURE);
}

void
usage(FILE *stream, const char *programName)
{
	fprintf(stream, "Usage: %s text-file string\n", programName);
	fprintf(stream, "Replace at most %d bytes of text-file in-memory with the given text."
					"The file remains unchanged\n", BYTES_READ);
}

int
main(int argc, char *argv[])
{
	/* Ensure sufficient arguments were provided */
	if (argc > 1 && (strcmp("--help", argv[1]) == 0 || strcmp("-h", argv[1]) == 0)) {
		usage(stdout, argv[0]);
		exit(EXIT_SUCCESS);
	}

	if (argc != 3) {
		usage(stderr, argv[0]);
		exit(EXIT_FAILURE);
	}

	/* Open input file; we are not writing to it, but O_RDWR is needed for PROT_WRITE on mapping */
	int fd = open(argv[1], O_RDWR);
	if (fd == -1)
		unix_error(true, "open() - Failed to open %s", argv[1]);
	
	struct stat statBuf;
	if (fstat(fd, &statBuf) == -1)
		unix_error(true, "fstat() - Failed to stat %s", argv[1]);
	if (statBuf.st_size < BYTES_READ) {
		fprintf(stderr, "Input file %s did not have at least %d bytes\n", argv[1], BYTES_READ);
		exit(EXIT_FAILURE);
	}

	/* Create private file mapping */
	char *mmapAddr = mmap(NULL, BYTES_READ, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	if (mmapAddr == MAP_FAILED)
		unix_error(true, "mmap() - Failed to create private file mapping from %s", argv[1]);

	/* Read first few bytes from file */
	char text[BYTES_READ + 1];	/* Extra byte for null terminator */
	memcpy(text, mmapAddr, BYTES_READ);
	text[BYTES_READ] = '\0';
	printf("Read: %s\n", text);

	printf("\n");
	/* Overwrite memory region with given string */
	size_t minSize = strlen(argv[2]) >= BYTES_READ ? BYTES_READ : strlen(argv[2]);
	memcpy(mmapAddr, argv[2], minSize);
	memset(text, 0, BYTES_READ);
	memcpy(text, mmapAddr, BYTES_READ);
	printf("Overwrote with: %s\n", text);

	printf("\n");

	/* Discard changes */
	printf("Discarding modifications with madvise() MADV_DONTNEED\n");
	if (madvise(mmapAddr, BYTES_READ, MADV_DONTNEED) == -1)
		unix_error(true, "madvise() - Failed to advise kernel with MADV_DONTNEED");
	memcpy(text, mmapAddr, BYTES_READ);
	printf("Final contents: %s\n", text);
	

	exit(EXIT_SUCCESS);
}

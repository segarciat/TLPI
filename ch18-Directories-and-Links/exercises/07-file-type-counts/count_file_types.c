#define _XOPEN_SOURCE 500
#include <ftw.h> /* nftw, FTW */

#include <stdio.h> /* fprintf(), stderr */
#include <stdlib.h> /* exit(), EXIT_SUCCESS, EXIT_FAILURE */

static long reg = 0;
static long dir = 0;
static long chr = 0;
static long blk = 0;
static long lnk = 0;
static long fifo = 0;
static long sock = 0;
static long unknown = 0;

static int
count_file_types(const char *pathname, const struct stat *statbuf, int typeflag, struct FTW *ftwbuf) 
{
	if (typeflag == FTW_NS) {
		fprintf(stderr, "Could not stat: %s\n", pathname);
		unknown++;
	} else {
		switch(statbuf->st_mode & S_IFMT) {	/* Count file type */
			case S_IFREG: reg++; break;
			case S_IFDIR: dir++; break;
			case S_IFCHR: chr++; break;
			case S_IFBLK: blk++; break;
			case S_IFLNK: lnk++; break;
			case S_IFIFO: fifo++; break;
			case S_IFSOCK: sock++; break;
			default: 
				fprintf(stderr, "Unknown type for: %s\n", pathname);
				unknown++;
				break;
		}
	}
	return 0;
}

static void displayCount(long count, long total, char *type)
{
	if (count > 0)
		printf("%s:\t\t%ld (%.2f%%)\n", type, count, 100 * (count + 0.0) / total);
}

int
main(int argc, char *argv[])
{
	if (argc > 2) {
		fprintf(stderr, "Usage: %s path\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	int flags = FTW_PHYS;	/* do not dereference symbolic links */
	int nopenfd = 20;
	if (nftw((argc == 2) ? argv[1] : ".", count_file_types, nopenfd, flags) == -1) {
		fprintf(stderr, "error during file tree walk\n");
		exit(EXIT_FAILURE);
	}
	long total = reg + dir + chr + blk + lnk + fifo + sock + unknown;
	displayCount(reg, total, "regular");
	displayCount(dir, total, "directory");
	displayCount(chr, total, "char device");
	displayCount(blk, total, "block device");
	displayCount(lnk, total, "symlink");
	displayCount(fifo, total, "fifo");
	displayCount(sock, total, "socket");
	displayCount(unknown, total, "unknown");
	printf("total: %ld\n", total);
	exit(EXIT_SUCCESS);
}

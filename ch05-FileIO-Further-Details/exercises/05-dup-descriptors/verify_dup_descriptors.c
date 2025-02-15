#include "tlpi_hdr.h"
#include <fcntl.h>
#include <unistd.h>

int
main(int argc, char* argv[])
{
	if (argc != 2 || strcmp(argv[1], "--help") == 0)
		usageErr("%s file\n", argv[0]);

	int fd = open(argv[1], O_RDONLY);
	if (fd == -1)
		errExit("open");
	
	int dupfd = dup(fd);
	if (dupfd == -1)
		errExit("dup");
	
	int oldFlags, dupFlags;
	if ((oldFlags = fcntl(fd, F_GETFL)) == -1 || (dupFlags = fcntl(dupfd, F_GETFL)) == -1)
		errExit("fcntl GETFL");
	if (oldFlags != dupFlags)
		errExit("duplicate file descriptor flags differ");
	
	ssize_t eofOffset = lseek(fd, 0, SEEK_END);
	if (eofOffset == -1)
		errExit("lseek");
	
	ssize_t curDupOffset = lseek(dupfd, 0, SEEK_CUR);
	if (curDupOffset == -1)
		errExit("lseek");
	if (eofOffset != curDupOffset)
		errExit("Offsets differ");
	printf("Flags and offsets match!\n"
			"Flags: %d\nOffsets:%lld\n", oldFlags, (long long) eofOffset);
    if (close(fd) == -1)
        errExit("close");
    exit(EXIT_SUCCESS);
}

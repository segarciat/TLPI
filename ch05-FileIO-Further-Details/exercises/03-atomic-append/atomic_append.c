#include <sys/stat.h>   // mode flags
#include <fcntl.h>      // open
#include <unistd.h>     // lseek, write
#include "tlpi_hdr.h"

/*
 * Exercise 5-3
 *
 * Demonstrates the race condition that the O_APPEND flag of the open()
 * system call is trying to address.
 *
 * Writes num_bytes to the specified file one at a time. If less than 3 arguments are
 * provided, then it opens with O_APPEND; otherwise, that flag is omitted,
 * but an lseek call is made past the end of the file prior to the write.
 */
int
main(int argc, char* argv[])
{
    if (argc < 3 || argc > 4 || (argc == 4 && strcmp(argv[3], "x") != 0))
        usageErr("%s file num_bytes [x]\n", argv[0]);

    long numBytes = getLong(argv[2], GN_ANY_BASE | GN_GT_0, argv[2]);
    int openFlags = O_WRONLY | O_CREAT;
    if (argc < 4)
        openFlags |= O_APPEND;
    
    int fd = open(argv[1], openFlags, S_IRUSR | S_IWUSR);
    if (fd == -1)
        errExit("open");
    
    while (numBytes-- > 0) {
        if (argc == 4 && lseek(fd, 0, SEEK_END) == -1)
            errExit("lseek");
        if (write(fd, "a", 1) == -1)
            errExit("write");
    }

    if (close(fd) == -1)
        errExit("close");
    exit(EXIT_SUCCESS);
}

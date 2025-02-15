#include <stdio.h>      // fprintf, stderr
#include <stdlib.h>     // exit, EXIT_FAILURE, EXIT_SUCCESS
#include <string.h>     // strcmp
#include <sys/types.h>  // off_t, and for portability
#include <fcntl.h>      // open
#include <unistd.h>     // lseek
#include <string.h>     // strlen, strcmp
#include "tlpi_hdr.h"


/*
    TLPI Exercise 05-02: 
    Opens a file in append mode, seeks to the beginning, and writes some content.

    append_seek: Demonstrates that when the O_APPEND flag is set on a file,
    writes are performed at the end of the file, even if a write is preceded
    by a call to lseek that repositions the file offset of the corresponding
    open file description.
*/

int
main(int argc, char* argv[])
{
    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s existing_file\n", argv[0]);

    // Open and check the file offset
    int fd = open(argv[1], O_WRONLY | O_APPEND);
    if (fd == -1)
        errExit("open");
    off_t filePos = lseek(fd, 0, SEEK_CUR);
    if (filePos == -1)
        errExit("lseek");
    printf("After opening file, the file offset is at: %lld\n", (long long) filePos);

    // Seek to the beginning before writing to file
    filePos = lseek(fd, 0, SEEK_SET);
    if (filePos == -1)
        errExit("lseek");
    printf("After seeking to start, the file offset is at: %lld\n", (long long) filePos);

    const char* text = "\nfirst\nsecond\n";
    if (write(fd, text, strlen(text)) == -1)
        errExit("write");

    filePos = lseek(fd, 0, SEEK_CUR);
    if (filePos == -1)
        errExit("lseek");
    printf("After writing file, the file offset is at: %lld\n", (long long) filePos);
    if (close(fd) == -1)
        errExit("close");

    exit(EXIT_SUCCESS);
}

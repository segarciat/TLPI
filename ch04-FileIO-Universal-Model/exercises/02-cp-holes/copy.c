#include <stdlib.h>     // exit, EXIT_SUCCESS
#include <sys/types.h>  // For portability
#include <unistd.h>     // read, write, lseek
#include <fcntl.h>      // open, open flags
#include <sys/stat.h>   // Mode (permission) flags
#include "tlpi_hdr.h"

#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif

// rw-r--r--
#define DEFAULT_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

/* copy: copy input file to output file, preserving holes */

int
main(int argc, char *argv[])
{
    if (argc != 3)
        usageErr("%s <old-file> <new-file>\n", argv[0]);
        
    /* Open input and output files */
    int inputFd = open(argv[1], O_RDONLY);
    if (inputFd == -1)
        errExit("opening input file");

    int outputFd = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, DEFAULT_PERMS);
    if (outputFd == -1)
        errExit("opening output file");

    /* Transfer data until we encounter EOF or error */
    char buf[BUF_SIZE];
    ssize_t numRead;
    while ((numRead = read(inputFd, buf, BUF_SIZE)) > 0) {
        // Heuristic: Any sequence of null bytes is a hole
        ssize_t nullByteCount = 0;
        while (nullByteCount < numRead && buf[nullByteCount] == '\0')
            nullByteCount++;
        // Advance past hole if necessary
        if (nullByteCount != 0 && lseek(outputFd, (off_t) nullByteCount, SEEK_CUR) == -1)
            errExit("lseek output file");
        if (nullByteCount != numRead) {
            ssize_t numToWrite = numRead - nullByteCount;
            if (write(outputFd, buf, (size_t) numToWrite) != numToWrite)
                errExit("write to output file");
        }
    }
    if (numRead == -1)
        errExit("read error");

    if (close(inputFd) == -1)
        errExit("close input file");
    if (close(outputFd) == -1)
        errExit("close output file");
    
    exit(EXIT_SUCCESS);
}

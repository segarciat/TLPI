#define _XOPEN_SOURCE   // getopt()

#include <stdlib.h>     // EXIT_FAILURE
#include <stdio.h>      // printf, fprintf, perror
#include <sys/types.h>  // For portability
#include <unistd.h>     // getopt, ssize_t
#include <sys/stat.h>   // Mode (permissions) flags
#include <fcntl.h>      // open() and its access mode flags
#include <stdbool.h>    // bool, true, false
#include "tlpi_hdr.h"   // errExit(), usageErr()

/* Enable the user to provide BUF_SIZE with -D when compiling with gcc. */
#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif

/* rw-r--r-- permissions by default */
#define DEFAULT_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

/* tee: read stdin and write to both stdout and a named file.

        By default, any content in the existing file is overwritten.
        If -a is specified, data is instead appended to the end of the file.
*/
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
                cmdLineErr("Unsupported option: %c\n", opt);
                break;
        }
    }

    /* Enforce that exactly one argument, a filename is provided */
    if (optind != argc - 1)
        usageErr("%s file\n", argv[0]);

    int flags = O_WRONLY | O_CREAT;
    flags |= (append) ? O_APPEND : O_TRUNC;

    int fd = open(argv[optind], flags, DEFAULT_PERMS);
    if (fd == -1)
        errExit("open");

    /* Read stdin */
    char buf[BUF_SIZE];
    ssize_t numRead;
    while ((numRead = read(STDIN_FILENO, buf, BUF_SIZE)) > 0) {
        /* Write to specified file */
        if (write(fd, buf, (size_t) numRead) != numRead)
            errExit("write");
        /* Write to stdout */
        if(write(STDOUT_FILENO, buf, (size_t) numRead) != numRead)
            errExit("write");
    }
    if (numRead == -1)
        errExit("read");
    if (close(fd) == -1)
        errExit("close");
    
    exit(EXIT_SUCCESS);
}

#include <sys/stat.h>   // mode flags
#include <fcntl.h>      // open
#include <stdio.h>      // NULL, printf
#include <string.h>     // strcmp
#include "tlpi_hdr.h"
#include "my_dup.h"

int main(int argc, char *argv[])
{
    if (argc != 2 || strcmp("--help", argv[1]) == 0)
        usageErr("%s file\n", argv[0]);

    int fd = open(argv[1], O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
    if (fd == -1)
        errExit("open");

    // Duplicate stdout on the fd
    int stdoutFd = my_dup2(fd, STDOUT_FILENO);
    if (stdoutFd == -1)
        errExit("my_dup2");

    const char *message = "Hello, world\n";
    size_t numToWrite = strlen(message);
    if (write(STDOUT_FILENO, message, numToWrite) != (ssize_t) numToWrite)
        fatal("write");

    if (close(fd) == -1) 
        errExit("close");

    exit(EXIT_SUCCESS);
}

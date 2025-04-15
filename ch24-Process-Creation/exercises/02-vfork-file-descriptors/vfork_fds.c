#define _DEFAULT_SOURCE /* Expose vfork() */
#include <unistd.h>     /* vfork() */
#include <stdlib.h>     /* exit(), EXIT_SUCCESS, EXIT_FAILURE */
#include <unistd.h>     /* write(), read(), close() */

#define BUF_SIZE 4096

int
main()
{
    pid_t childPid;
    /* fork */
    childPid = vfork();
    ssize_t numWritten;
    switch(childPid) {
    case -1: /* error */
        numWritten = write(STDERR_FILENO, "vfork failed\n", 14);
        (void) numWritten;
        exit(EXIT_FAILURE);
    case 0: /* child */
        if (close(STDIN_FILENO) == -1) {
            /* Display some error message */
            numWritten = write(STDERR_FILENO, "Error in child\n", 16);
            (void) numWritten;
            _exit(EXIT_FAILURE);
        }
        _exit(EXIT_SUCCESS);
    default:
        /* Try operating with file descriptor */
        if (write(STDOUT_FILENO, "Provide some input: ", 21) == -1) {
            numWritten = write(STDERR_FILENO, "Error in parent\n", 17);
            (void) numWritten;
            exit(EXIT_FAILURE);
        }
        char buf[BUF_SIZE];
        ssize_t numRead;
        if ((numRead = read(STDIN_FILENO, buf, BUF_SIZE)) == -1) {
            numWritten = write(STDERR_FILENO, "Error reading in parent", 24);
            (void) numWritten;
            exit(EXIT_FAILURE);
        }

        if (write(STDOUT_FILENO, buf, (size_t) numRead) != numRead) {
            numWritten = write(STDERR_FILENO, "Error echoing from parent", 26);
            (void) numWritten;
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }
}

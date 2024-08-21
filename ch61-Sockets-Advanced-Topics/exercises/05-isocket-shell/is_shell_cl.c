#include "is_shell.h"

#define MAX_OUTPUT_SIZE 4096

int
main(int argc, char* argv[])
{
    if (argc != 3 || strcmp("--help", argv[0]) == 0)
        usageErr("%s host command-string\n"
                 "Executes the given command of at most %ld characters in length on the named host\n",
                 argv[0], MAX_COMMAND_SIZE);

    char *host = argv[1];
    int cfd = inetConnect(host, PORT_NUM, SOCK_STREAM);
    if (cfd == -1)
        errExit("inetConnect(): Failed to connect to host");
    char *command = argv[2];

    size_t commandLen  = strlen(command);
    if (writen(cfd, command, commandLen) != commandLen)
        errExit("write(): Failed to write command to server");
    if (shutdown(cfd, SHUT_WR) == -1) /* Close write end of socket so server sees EOF */
        errExit("shutdown(): Failed to close write end of connection");

    char commandOutput[MAX_OUTPUT_SIZE];
    ssize_t bytesRead;
    for (;;){ 
        bytesRead = readn(cfd, commandOutput, MAX_OUTPUT_SIZE);
        if (bytesRead == -1)
            errExit("readn(): Failed to read command output from server");
        if (bytesRead == 0)
            exit(EXIT_SUCCESS);
        if (writen(STDOUT_FILENO, commandOutput, bytesRead) != bytesRead)
            fatal("writen(): Failed to write command output to standard output");
    }
    if (write(STDOUT_FILENO, "\n", 1) == -1)
        exit(EXIT_FAILURE);
    exit(EXIT_SUCCESS);
}

#include "isock_cat.h"

#define BUF_SIZE 4096

int
main(int argc, char *argv[])
{
    int sfd = inetListen(PORT_NUM, SOCK_STREAM, NULL);
    if (sfd == -1)
        errExit("inetListen(): Failed to create at port %s", PORT_NUM);

    pid_t pid = getpid();
    printf("[%ld] Listening  on port %s\n", (long) pid, PORT_NUM);
    char buf [BUF_SIZE];
    char clAddrStr[IS_ADDR_STR_LEN];
    for (;;) {
        struct sockaddr_storage clAddr;
        socklen_t clLen = sizeof(struct sockaddr_storage);
        int cfd = accept(sfd, (struct sockaddr *) &clAddr, &clLen);
        if (cfd == -1) {
            errMsg("accept(): Failed to accept connection; skipping\n");
            continue;
        }
        inetAddressStr((struct sockaddr *) &clAddr, clLen, clAddrStr, IS_ADDR_STR_LEN);
        printf("Accepted connection from: %s\n", clAddrStr);
        
        ssize_t bytesRead;
        while ((bytesRead = read(cfd, buf, BUF_SIZE)) > 0)
            if (write(STDOUT_FILENO, buf, bytesRead) != bytesRead)
                fatal("write(): Error writing to stdout");
        if (bytesRead == -1) {
            errMsg("read(): Error reading from connected socket; abandoning\n");
            continue;
        }
    }
}

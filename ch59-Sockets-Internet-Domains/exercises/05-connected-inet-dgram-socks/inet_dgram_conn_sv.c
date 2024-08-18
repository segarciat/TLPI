#include "inet_dgram_conn.h"

#define PORT_NUM_B "50001"
#define BUF_SIZE 100

int
main(int argc, char *argv[])
{
    /* Create two sockets bound to a different port */
    int sfdA = inetBind(PORT_NUM_A, SOCK_DGRAM, NULL);
    if (sfdA == -1)
        errExit("Failed to create datagram socket bound to port %s", PORT_NUM_A);
    printf("Created socket bounded at port %s\n", PORT_NUM_A);

    socklen_t addrLenB;
    int sfdB = inetBind(PORT_NUM_B, SOCK_DGRAM, &addrLenB);
    if (sfdB == -1)
        errExit("Failed to create datagram socket bound to port %s", PORT_NUM_B);

    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    struct addrinfo* result;
    int s = getaddrinfo("localhost", PORT_NUM_B, &hints, &result);
    if (s != 0)
        fatal("Failed to resolve server at port %s: %s\n", PORT_NUM_B, gai_strerror(s));

    struct addrinfo *rp;
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        if (connect(sfdA, rp->ai_addr, rp->ai_addrlen) != -1)
            break;
    }
    if (rp == NULL)
        errExit("Failed to connect to socket at port %s\n", PORT_NUM_B);
    freeaddrinfo(result);

    char clAddrStr[IS_ADDR_STR_LEN];
    char buf [BUF_SIZE];
    for (;;) {
        socklen_t len = sizeof(struct sockaddr_storage);
        struct sockaddr_storage claddr;
        ssize_t numBytes = recvfrom(sfdA, buf, BUF_SIZE, 0,
                                    (struct sockaddr *) &claddr, &len);
        if (numBytes == -1)
            errExit("recvfrom() failed to retrieve bytes from socket");

        inetAddressStr((struct sockaddr *) &claddr, len, clAddrStr, IS_ADDR_STR_LEN);
        printf("Received %ld bytes from %s\n", (long) numBytes, clAddrStr);
    }
}

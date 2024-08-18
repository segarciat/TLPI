#include "so_dgram_conn.h"

#define SECOND_SOCKET_PATH "/tmp/so_dgram_conn_b"

int
createDgramSocket(const char *sockPath, struct sockaddr_un* addrp)
{
    /* Create socket */
    int sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sfd == -1)
        errorExit("Failed to create socket");

    /* Build socket address */
    if (strlen(sockPath) > sizeof(addrp->sun_path) -1) {
        fprintf(stderr, "Socket path is too long\n");
        exit(EXIT_FAILURE);
    }

    if (remove(sockPath) == -1 && errno != ENOENT)
        errorExit("Failed to remove socket file");

    memset(addrp, 0, sizeof(struct sockaddr_un));
    addrp->sun_family = AF_UNIX;
    strncpy(addrp->sun_path, sockPath, sizeof(addrp->sun_path) - 1);

    if (bind(sfd, (struct sockaddr *) addrp, sizeof(struct sockaddr_un)) == -1)
        errorExit("Failed to bind socket address");

    return sfd;
}

int
main(int argc, char *argv[])
{
    struct sockaddr_un addrA;
    int sfdA = createDgramSocket(SV_SOCKET_PATH, &addrA);

    struct sockaddr_un addrB;
    int sfdB = createDgramSocket(SECOND_SOCKET_PATH, &addrB);

    if (connect(sfdA, (struct sockaddr *) &addrB, sizeof(struct sockaddr_un)) == -1)
        errorExit("Failed to connect socket A to socket B");

    /* Receive messages */
    char buf[BUF_SIZE];
    for (;;) {
        printf("Waiting for message on socket A\n");
        socklen_t len = sizeof(struct sockaddr_un);
        struct sockaddr_un claddr;
        ssize_t numBytes = recvfrom(sfdA, buf, BUF_SIZE, 0, (struct sockaddr *) &claddr, &len);
        if (numBytes == -1)
            errorExit("Failed to read data from socket");

        printf("Server received %ld bytes on socket %s from client %s\n",
                    (long) numBytes, SV_SOCKET_PATH, SECOND_SOCKET_PATH);
        
        printf("Waiting for message on socket B\n");
        numBytes = recvfrom(sfdB, buf, BUF_SIZE, 0, (struct sockaddr *) &claddr, &len);
        if (numBytes == -1)
            errorExit("Failed to read data from socket");

        printf("Server received %ld bytes on socket %s from client %s\n",
                    (long) numBytes, SV_SOCKET_PATH, SECOND_SOCKET_PATH);
    }

    exit(EXIT_SUCCESS);
}

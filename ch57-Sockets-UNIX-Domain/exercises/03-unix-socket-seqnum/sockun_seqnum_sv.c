#include "sockun_seqnum.h"

int
main(int argc, char *argv[])
{
    /* Create UNIX domain socket */
    int sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1)
        errorExit("Failed to create socket");

    /* Construct well-known address and bind server socket to it */
    struct sockaddr_un svaddr;
    if (strlen(SV_SOCK_PATH) > sizeof(svaddr.sun_path) - 1) {
        fprintf(stderr, "Server socket path is too long\n");
        exit(EXIT_FAILURE);
    }

    if (remove(SV_SOCK_PATH) == -1 && errno != ENOENT)
        errorExit("Failed to remove server socket path");

    memset(&svaddr, 0, sizeof(struct sockaddr_un));
    svaddr.sun_family = AF_UNIX;
    strncpy(svaddr.sun_path, SV_SOCK_PATH, sizeof(svaddr.sun_path) - 1);

    if (bind(sfd, (struct sockaddr *) &svaddr, sizeof(struct sockaddr_un)) == -1)
        errorExit("Failed to bind socket");

    if (listen(sfd, BACKLOG) == -1)
        errorExit("Failed to mark socket as passive");

    /* Wait for client connections */
    unsigned long seqnum = 0;
    char buf[BUF_SIZE];
    for (;;) {
        /* Accept client conection */
        struct sockaddr_un claddr;
        socklen_t claddrLen = sizeof(struct sockaddr_un);
        int cfd = accept(sfd, (struct sockaddr *) &claddr, &claddrLen);
        if (cfd == -1)
            errorExit("Failed to accept client conection");

        /* Read client's request */
        ssize_t bytesRead = read(cfd, buf, BUF_SIZE);
        if (bytesRead == -1)
            errorExit("Failed to read client data from socket");
        if (bytesRead == 0)
            continue;       /* No data read; skip */

        /* Parse number of sequence numbers desired */
        buf[bytesRead-1] = '\0';
        long requestCount;
        if (parseLong(buf, &requestCount) == -1 || requestCount <= 0)
            continue;   /* Bad request; skip */

        /* Assign sequence number to client */
        memset(buf, 0, BUF_SIZE);
        snprintf(buf, BUF_SIZE, "%lu", seqnum);
        if (write(cfd, buf, BUF_SIZE) != BUF_SIZE) {
            fprintf(stderr, "Failed to write sequence number to socket\n");
            exit(EXIT_FAILURE);
        }
        seqnum += requestCount;
    }

    exit(EXIT_SUCCESS);
}

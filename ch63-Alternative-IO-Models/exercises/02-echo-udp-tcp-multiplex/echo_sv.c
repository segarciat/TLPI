#include <poll.h>
#include <unistd.h>     /* close() */
#include <fcntl.h>
#include "echo_multiplex.h"

#define BACKLOG 5


/* Service a TCP client using connected file descriptor cfd. */
static void
serviceEchoTcpRequest(int cfd)
{
    char buf[BUF_SIZE];
    ssize_t bytesRead;

    for (;;) {
        bytesRead = readn(cfd, buf, BUF_SIZE);
        if (bytesRead == -1)
            errExit("readn(): Failed to create from socket");
        if (bytesRead ==  0) /* EOF */
            break;

        if (writen(cfd, buf, bytesRead) != bytesRead)
            fatal("writen(): Failed to echo back to client");
    }
    if (close(cfd) == -1)
        errExit("close(): Failed to close client connection");
}

/* Accepts a TCP connection and forks a child to service the request. */
static void
acceptTcpConn(int tcpLfd)
{
    int cfd = accept(tcpLfd, NULL, NULL);
    if (cfd == -1) {
        errMsg("Error accepting client connection; skipping\n");
        return;
    }
    switch (fork()) {
        case  -1:       /* Possibly temporary error  */
            errMsg("Error forking child; skipping");
            close(cfd);
            break;
        case   0:       /* Child */
            close(tcpLfd);  /* Close unneeded descriptor */
            serviceEchoTcpRequest(cfd);
            exit(EXIT_SUCCESS);
        default:
            close(cfd);     /* Close unneeded descriptor */
            break;
    }
}

/* Handle datagram on UDP socket */
static void
handleDgram(int udpSfd)
{
    char buf[BUF_SIZE];
    struct sockaddr_storage clAddr;
    socklen_t clAddrLen = sizeof(struct sockaddr_storage);
    ssize_t bytesRead = recvfrom(udpSfd, buf, BUF_SIZE, 0, (struct sockaddr *) &clAddr, &clAddrLen);
    if (bytesRead == -1) {
        errMsg("Failed to receive message; skipping\n");
        return;
    }

    char clAddrStr[IS_ADDR_STR_LEN];
    inetAddressStr((struct sockaddr *) &clAddr, clAddrLen, clAddrStr, IS_ADDR_STR_LEN);
    printf("Datagram from: %s\n", clAddrStr);

    if (sendto(udpSfd, buf, bytesRead, 0, (struct sockaddr *) &clAddr, clAddrLen) == -1)
        errMsg("Failed to reply  to client; skipping\n");
}

int
main(int argc, char* argv[])
{
    struct pollfd pollFds[2];
    int tcpFlags, udpFlags;

    /* Create TCP socket */
    int tcpLfd = inetListen(SERVICE, BACKLOG, NULL);
    if (tcpLfd == -1)
        errExit("inetListen(): Failed to create listening socket at %s", SERVICE);

    tcpFlags = fcntl(tcpLfd, F_GETFL);
    if (tcpFlags == -1)
        errExit("fcntl() F_GETFL: Failed to retrieve tcp socket flags");
    if (fcntl(tcpLfd, F_SETFL, tcpFlags | O_NONBLOCK) == -1)
        errExit("fcntl() F_SETFL: Failed to set nonblock flag on tcp listening descriptor");

    pollFds[0].fd = tcpLfd;
    pollFds[0].events = POLLIN;

    /* Create UDP socket */
    int udpSfd = inetBind(SERVICE, SOCK_DGRAM, NULL);
    if (udpSfd ==  -1)
        errExit("inetBind(): Failed to bind datagram socket at %s", SERVICE);

    udpFlags = fcntl(udpSfd, F_GETFL);
    if (udpFlags == -1)
        errExit("fcntl() F_GETFL: Failed to retrieve udp socket flags");
    if (fcntl(tcpLfd, F_SETFL, udpFlags | O_NONBLOCK) == -1)
        errExit("fcntl() F_SETFL: Failed to set nonblock flag on udp socket descriptor");

    pollFds[1].fd = udpSfd;
    pollFds[1].events = POLLIN;

    /* Perform multiplexed I/O */
    for (;;) {
        int ready = poll(pollFds, 2, -1);
        if (ready == -1)
            errExit("poll(): Failed to determine readiness of file descriptors");

        if (pollFds[0].revents & POLLIN)    /* New TCP connection */
            acceptTcpConn(tcpLfd);

        if (pollFds[1].revents & POLLIN)    /* Socket datagram available */
            handleDgram(udpSfd);
    }
}

#include "sun_dgram.h"

#define DELAY_SECS 1

int
main(int argc, char *argv[])
{

    int sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sfd == -1)
        errorExit("Failed to create socket");

    struct sockaddr_un svaddr;
    /* Construct well-known address and bind server socket to it */
    if (strlen(SV_SOCKET_PATH) > sizeof(svaddr.sun_path) - 1)
        errorExit("Server socket path is too long");
    if (remove(SV_SOCKET_PATH) == -1 && errno != ENOENT)
        errorExit("Failed to remove socket file");

    memset(&svaddr, 0, sizeof(struct sockaddr_un));
    svaddr.sun_family = AF_UNIX;
    strncpy(svaddr.sun_path, SV_SOCKET_PATH, sizeof(svaddr.sun_path) - 1);

    if (bind(sfd, (struct sockaddr *) &svaddr, sizeof(struct sockaddr_un)) == -1)
        errorExit("Failed to bind socket");

    char buf[BUF_SIZE];
    for (;;) {
        /* Introduce an artificial delay between reads */
        struct timespec ts;
        if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
            errorExit("Failed to get current time");
        ts.tv_sec += DELAY_SECS;
        int s = clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &ts, NULL);
        if (s != 0) {
            fprintf(stderr, "Failed to sleep complete duration: %s\n", strerror(s));
            exit(EXIT_FAILURE);
        }

        /* Read data from socket; don't store client's address */
        ssize_t numBytes = recvfrom(sfd, buf, BUF_SIZE, 0, NULL, NULL);
        if (numBytes == -1)
            errorExit("Failed to read from socket wtih recvfrom");
        if (write(STDOUT_FILENO, buf, numBytes) != numBytes) {
            fprintf(stderr, "Failed to write all the data received\n");
            exit(EXIT_FAILURE);
        }
    }

    exit(EXIT_SUCCESS);
}

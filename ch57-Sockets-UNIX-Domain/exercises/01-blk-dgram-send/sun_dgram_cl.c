#include "sun_dgram.h"
#include <sys/types.h>  /* pid_t */

int main(int argc, char* argv[])
{
    /* Create socket */
    int sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sfd == -1)
        errorExit("Failed to open socket");

    /* Construct address of server */
    struct sockaddr_un svaddr;
    memset(&svaddr, 0, sizeof(struct sockaddr_un));
    svaddr.sun_family = AF_UNIX;
    strncpy(svaddr.sun_path, SV_SOCKET_PATH, sizeof(svaddr.sun_path) - 1);

    /* Send all data in stdin */
    char buf[BUF_SIZE];
    ssize_t bytesRead;
    pid_t pid = getpid();
    while ((bytesRead = read(STDIN_FILENO, buf, BUF_SIZE)) > 0) {
        if (sendto(sfd, buf, bytesRead, 0, (struct sockaddr *) &svaddr,
                sizeof(struct sockaddr_un)) != bytesRead) {
            fprintf(stderr, "Failed to send all data read\n");
            exit(EXIT_FAILURE);
        }
        struct timespec ts;
        if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
            errorExit("Failed to get current time");
        printf("[%ld]: Sent %lld bytes at: %s", (long) pid, (long long) bytesRead, ctime(&ts.tv_sec));
    }
    if (bytesRead == -1)
        errorExit("Failed to read data from stdin");
    exit(EXIT_SUCCESS);
}

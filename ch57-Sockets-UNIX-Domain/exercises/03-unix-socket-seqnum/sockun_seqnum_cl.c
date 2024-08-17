#include "sockun_seqnum.h"
#include <limits.h>

/* Writes usage message to the specified stream. */
static void
displayUsage(FILE *stream, const char* programName)
{
    fprintf(stream, "Usage: %s count\n", programName);
    fprintf(stream, "Requests as many sequence numbers as specified by 'count'.\n");
}

int
main(int argc, char* argv[])
{
    if (argc > 1 && strcmp("--help", argv[1]) == 0) {
        displayUsage(stdout, argv[0]);
        exit(EXIT_SUCCESS);
    }

    if (argc != 2) {
        displayUsage(stderr, argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Parse given integer */
    if (strlen(argv[1]) > BUF_SIZE - 1) {
        fprintf(stderr, "Input too long\n");
        exit(EXIT_FAILURE);
    }
    char buf[BUF_SIZE];
    memset(buf, 0, BUF_SIZE);
    memcpy(buf, argv[1], strlen(argv[1]));

    long n = 0;
    if (parseLong(argv[1], &n) == -1 || n <= 0) {
        fprintf(stderr, "%s: Failed to parse as valid count: %s", argv[0], argv[1]);
        if (errno != 0)
            fprintf(stderr, "\n%s: %s", argv[0], strerror(errno));
        fprintf(stderr, "\n");

        exit(EXIT_FAILURE);
    }

    /* Create client socket */
    int sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1)
        errorExit("Failed to create socket");

    /* Construct server's socket address */
    struct sockaddr_un svaddr;
    memset(&svaddr, 0, sizeof(struct sockaddr_un));
    svaddr.sun_family = AF_UNIX;
    strncpy(svaddr.sun_path, SV_SOCK_PATH, sizeof(svaddr.sun_path) - 1);

    if (connect(sfd, (struct sockaddr *) &svaddr, sizeof(struct sockaddr_un))== -1)
        errorExit("Failed to connect to server socket");

    /* Request sequence number(s) */
    ssize_t bytesWritten = write(sfd, buf, BUF_SIZE);
    if (bytesWritten != BUF_SIZE) {
        fprintf(stderr, "Failed to write request to server socket\n");
        exit(EXIT_FAILURE);
    }

    ssize_t bytesRead = read(sfd, buf, BUF_SIZE);
    if (bytesRead != BUF_SIZE) {
        fprintf(stderr, "Failed to read all data from server socket\n");
        exit(EXIT_FAILURE);
    }
    buf[bytesRead - 1] = '\0';
    printf("%s\n", buf);
    exit(EXIT_SUCCESS);
}

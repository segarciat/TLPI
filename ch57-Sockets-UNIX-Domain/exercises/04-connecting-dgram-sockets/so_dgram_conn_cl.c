#include "so_dgram_conn.h"

static void
displayUsage(FILE *stream, const char *programName)
{
    fprintf(stderr, "Usage: %s text...\n", programName);
    fprintf(stderr, "Send one or more strings to the server. Server will display them on stdout.\n");
}

int
main(int argc, char *argv[])
{
    if (argc > 1 && strcmp("--help", argv[1]) == 0) {
        displayUsage(stdout, argv[0]);
        exit(EXIT_SUCCESS);
    }

    if (argc == 1) {
        displayUsage(stderr, argv[0]);
        exit(EXIT_SUCCESS);
    }

    int sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sfd == -1)
        errorExit("Failed to create socket");

    /* Construct server address and connect to it */
    struct sockaddr_un svaddr;
    svaddr.sun_family = AF_UNIX;
    strncpy(svaddr.sun_path, SV_SOCKET_PATH, sizeof(svaddr.sun_path) - 1);

    if (connect(sfd, (struct sockaddr *) &svaddr, sizeof(struct sockaddr_un)) == -1)
        errorExit("Failed to connect to server socket");

    for (int i = 1; i < argc; i++) {
        size_t msgLen = strlen(argv[i]);
        if (sendto(sfd, argv[i], msgLen, 0, (struct sockaddr *) &svaddr,
                    sizeof(struct sockaddr_un)) != msgLen) {
            fprintf(stderr, "Failed to write complete message to server socket\n");
            exit(EXIT_FAILURE);
        }
    }
    printf("Successfully sent all messages\n");
    exit(EXIT_SUCCESS);
}

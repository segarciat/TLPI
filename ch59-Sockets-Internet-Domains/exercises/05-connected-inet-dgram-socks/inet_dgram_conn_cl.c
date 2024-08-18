#include "inet_dgram_conn.h"

#define PORT_NUM "50002"

int
main(int argc, char *argv[])
{
    if (argc == 1 || strcmp("--help", argv[1]) == 0)
        usageErr("%s message...\n", argv[0]);

    int cfd = inetConnect("localhost", PORT_NUM_A, SOCK_DGRAM);
    if (cfd == -1)
        errExit("inetconnect()");

    for (int i = 1; i < argc; i++) {
        size_t msgLen = strlen(argv[i]);
        if (write(cfd, argv[i], msgLen) != msgLen)
            errExit("write() failed to send message : %s", argv[i]);
        printf("Sent %s successfully\n", argv[i]);
    }
    printf("Successfully sent all messages\n");
    exit(EXIT_SUCCESS);
}

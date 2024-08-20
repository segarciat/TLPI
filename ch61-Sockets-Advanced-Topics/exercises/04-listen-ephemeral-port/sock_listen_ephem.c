#define _GNU_SOURCE     /* Expose NI_MAXHOST, NI_MAXSERV from <netdb.h> */
#include <stdio.h>      /* fprintf(), printf(), stderr */
#include <stdlib.h>     /* exit(), EXIT_FAILURE, EXIT_SUCCESS */
#include <errno.h>      /* errno */
#include <string.h>     /* strerror() */
#include <sys/socket.h>
#include <netdb.h>      /* getaddrinfo(), getnameinfo(), NI_MAXHOST, NI_MAXSERV, NI_NUMERICSERV */
#include <unistd.h>     /* close() */

#define PORT_NUM "50000"
#define BACKLOG 5

static void
errorExit(const char *msg)
{
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
    /* Create a listening socket */
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    hints.ai_protocol = 0;          /* Any protocol */
    hints.ai_family = AF_UNSPEC;    /* IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE | AI_NUMERICHOST;

    struct addrinfo *result;
    int s = getaddrinfo(NULL, PORT_NUM, &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo(): %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }

    struct addrinfo *rp;
    int lfd;
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        lfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (lfd == -1)
            continue;
        int optval = 1;
        if (setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
            close(lfd);
            freeaddrinfo(result);
            errorExit("setsockopt(): Failed to set SO_REUSEADDR option");
        } else
            break;
    }

    freeaddrinfo(result);

    if (rp == NULL) {
        fprintf(stderr, "Failed to create socket\n");
        exit(EXIT_FAILURE);
    }

    if (listen(lfd, BACKLOG) == -1)
        errorExit("listen(): Failed to convert socket to passive socket");

    /* Find out port number */
    struct sockaddr_storage addr;
    socklen_t addrlen = sizeof(struct sockaddr_storage);
    if (getsockname(lfd, (struct sockaddr *) &addr, &addrlen) == -1)
        errorExit("getsockname(): Failed to get socket address");

    char host[NI_MAXHOST];
    char service[NI_MAXSERV];
    s = getnameinfo((struct sockaddr *) &addr, addrlen,
            host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICSERV);
    if (s != 0) {
       fprintf(stderr, "getnameinfo(): %s\n", gai_strerror(s));
       exit(EXIT_FAILURE);
    }
    printf("We specified port %s and were assigned: (%s, %s)\n", PORT_NUM, host, service);

    exit(EXIT_SUCCESS);
}

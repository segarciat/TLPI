#include "ud_sockets.h"
#include <errno.h>      /* errno, EINVAL, ENAMETOOLONG */
#include <stdio.h>      /* remove() */
#include <stdbool.h>    /* bool */
#include <string.h>     /* strnlen(), memset(), strncpy() */
#include <unistd.h>     /* close() */

int udConnect(const char *sockpath, int type)
{
    if (sockpath == NULL) {
        errno = EINVAL;
        return -1;
    }

    int cfd = socket(AF_UNIX, type, 0);
    if (cfd == -1)
        return -1;

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, sockpath, sizeof(addr.sun_path) - 1);
    
    if (connect(cfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_un)) == -1) {
        close(cfd);
        return -1;
    }
    return cfd;
}

static int
udPassiveSocket(const char *sockpath, int type, bool doListen, int backlog)
{
    if (sockpath == NULL) {
        errno = EINVAL;
        return -1;
    }
    struct sockaddr_un addr;
    size_t maxPathSize = sizeof(addr.sun_path);
    if (strnlen(sockpath, maxPathSize) >= maxPathSize) {
        errno = ENAMETOOLONG;
        return -1;
    }

    int sfd = socket(AF_UNIX, type, 0);
    if (sfd == -1)
        return -1;

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, sockpath, sizeof(addr.sun_path) - 1);

    if (bind(sfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_un)) == -1) {
        close(sfd);
        return -1;
    }

    if (doListen && listen(sfd, backlog) == -1) {
        close(sfd);
        return -1;
    } 
    return sfd;
}

int udListen(const char *sockpath, int backlog)
{
    return udPassiveSocket(sockpath, SOCK_STREAM, true, backlog);
}

int udBind(const char *sockpath, int type)
{
    return udPassiveSocket(sockpath, type, false, 0);
}

#include "isock_cat.h"
#include <fcntl.h>      /* open(), O_RDONLY */
#include <sys/stat.h>   /* struct stat */
#include "sendfile_rwl.h"

int
main(int argc, char *argv[])
{
    if (argc != 3 || strcmp("--help", argv[1]) == 0)
        usageErr("%s host filename\n", argv[0]);

    int fd = open(argv[2], O_RDONLY);
    if (fd == -1)
        errExit("open(): Failed to open for reading: %s", argv[2]);

    struct stat sb;
    if (fstat(fd, &sb) == -1)
        errExit("fstat(): Failed to stat %s", argv[2]);

    int cfd = inetConnect(argv[1], PORT_NUM, SOCK_STREAM);
    if (cfd == -1)
        errExit("inectConnect(): Failed to connect to %s at port %s", argv[1], PORT_NUM);

    if (sendfile_rwl(cfd, fd, NULL, sb.st_size) == -1)
        errExit("sendfile_rwl(): Failed to send %s to socket", argv[2]);
    exit(EXIT_SUCCESS);
}

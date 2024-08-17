#ifndef _H_SUN_DGRAM
#define _H_SUN_DGRAM

#define _POSIX_C_SOURCE 200112L /* Expose clock_gettime(), clock_nanosleep() in time.h */
#include <sys/un.h>             /* struct sockaddr_un */
#include <sys/socket.h>         /* struct sockaddr, socket(), bind(), AF_UNIX */
#include <stdio.h>              /* fprintf(), stderr, remove() */
#include <stdlib.h>             /* exit(), EXIT_FAILURE, EXIT_SUCCESS */
#include <errno.h>              /* errno */
#include <string.h>             /* strerror(), memset(), strncpy() */
#include <unistd.h>             /* read(), write(), STDIN_FILENO, STDOUT_FILENO */
#include <time.h>               /* ctime(), clock_gettime(), clock_nanosleep() */

#define SV_SOCKET_PATH "/tmp/sun_dgram_tlpi_57_1.sock"
#define BUF_SIZE 1024

void errorExit(const char *msg);

#endif

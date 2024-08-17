#ifndef _SO_DGRAM_CONN_H
#define _SO_DGRAM_CONN_H

#include <sys/un.h>     /* struct sockaddr_un */
#include <sys/socket.h> /* socket(), connect(), etc */
#include <stdlib.h>     /* exit(), EXIT_SUCCESS */
#include <stdio.h>      /* fprintf(), stderr */
#include <string.h>     /* memset(), strncpy(), strlen() */
#include <errno.h>      /* errno, ENOENT */

#define SV_SOCKET_PATH "/tmp/so_dgram_conn_a"
#define BUF_SIZE 128

void errorExit(const char *msg);

#endif

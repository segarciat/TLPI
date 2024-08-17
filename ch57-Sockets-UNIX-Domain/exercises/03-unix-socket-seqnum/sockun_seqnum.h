#ifndef _SOCKUN_SEQNUM_H
#define _SOCKUN_SEQNUM_H

#include <sys/un.h>         /* struct sockaddr_un */
#include <sys/socket.h>     /* socket() */
#include <stdlib.h>         /* exit(), EXIT_FAILURE, EXIT_SUCCESS */
#include <stdio.h>          /* fprintf(), stderr, remove() */
#include <string.h>         /* strerror(), strlen() */
#include <errno.h>          /* errno, ENOENT */
#include <unistd.h>         /* read(), write() */
#include <sys/types.h>      /* ssize_t */

#define SV_SOCK_PATH "/tmp/sockun_seqnum"
#define BUF_SIZE 32
#define BACKLOG 5

void errorExit(const char* msg);
int parseLong(const char* s, long* n);

#endif

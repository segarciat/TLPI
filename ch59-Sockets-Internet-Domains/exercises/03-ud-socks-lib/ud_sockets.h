#ifndef _UNIX_DOMAIN_SOCKETS_H
#define _UNIX_DOMAIN_SOCKETS_H

#include <sys/un.h>
#include <sys/socket.h>

/* Creates a socket of the given type connected to the socket path specified.
   On success, returns the connected file descriptor. On failure, returns -1.
*/
int udConnect(const char *sockpath, int type);

/* Creates a listening stream socket bound to the provided socket path. 
   On success, returns the listening file descriptor. On failure, returns -1.
*/
int udListen(const char *sockpath, int backlog);

/* Creates a socket of the given type bound to the provided socket path.
   On success, returns the socket file descriptor. On failure, returns -1.
*/
int udBind(const char *sockpath, int type);

#endif

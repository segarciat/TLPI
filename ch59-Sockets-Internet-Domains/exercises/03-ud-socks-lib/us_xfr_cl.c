/* Modified by Sergio E. Garcia Tapia for Exercise 59-3 */

/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2023.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Exercise 59-3 (Listing 57-4) */

/* us_xfr_cl.c

   An example UNIX domain stream socket client. This client transmits contents
   of stdin to a server socket.

   See also us_xfr_sv.c.
*/
#include "us_xfr.h"

int
main(int argc, char *argv[])
{

    /* Create a socket connected to server address */
    int cfd = udConnect(SV_SOCK_PATH, SOCK_STREAM);
    if (cfd == -1)
        errExit("Failed to create active stream socket in UNIX domain connected to %s", SV_SOCK_PATH);

    /* Copy stdin to socket */

    ssize_t numRead;
    char buf[BUF_SIZE];
    while ((numRead = read(STDIN_FILENO, buf, BUF_SIZE)) > 0)
        if (write(cfd, buf, numRead) != numRead)
            fatal("partial/failed write");

    if (numRead == -1)
        errExit("read");

    exit(EXIT_SUCCESS);         /* Closes our socket; server sees EOF */
}

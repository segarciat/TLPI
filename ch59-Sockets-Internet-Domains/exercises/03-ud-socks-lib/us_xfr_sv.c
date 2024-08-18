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

/* Exercise 59-3 (based on Listing 57-3) */

/* us_xfr_sv.c

   An example UNIX stream socket server. Accepts incoming connections
   and copies data sent from clients to stdout.

   See also us_xfr_cl.c.
*/
#include "us_xfr.h"
#define BACKLOG 5

int
main(int argc, char *argv[])
{

    if (remove(SV_SOCK_PATH) == -1 && errno != ENOENT)
        errExit("remove-%s", SV_SOCK_PATH);
    
    /* Construct server socket address, bind socket to it,
       and make this a listening socket */
    int lfd = udListen(SV_SOCK_PATH, BACKLOG);
    if (lfd == -1)
        errExit("udListen() - Failed to create listening stream socket in UNIX domain at %s", SV_SOCK_PATH);

    char buf[BUF_SIZE];
    for (;;) {          /* Handle client connections iteratively */

        /* Accept a connection. The connection is returned on a new
           socket, 'cfd'; the listening socket ('lfd') remains open
           and can be used to accept further connections. */

        int cfd = accept(lfd, NULL, NULL);
        if (cfd == -1)
            errExit("accept");

        /* Transfer data from connected socket to stdout until EOF */

        ssize_t numRead;
        while ((numRead = read(cfd, buf, BUF_SIZE)) > 0)
            if (write(STDOUT_FILENO, buf, numRead) != numRead)
                fatal("partial/failed write");

        if (numRead == -1)
            errExit("read");

        if (close(cfd) == -1)
            errMsg("close");
    }
}

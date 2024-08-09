/* Modified by Sergio E. Garcia Tapia for Exercise 54-1 */

/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2023.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Exercise 54-1 (based on Listing 48-1) */

/*  pshm_xfr.h

   Header file used by the pshm_xfr_reader.c and pshm_xfr_writer.c programs.
*/
#include "tlpi_hdr.h"
#include <sys/types.h>  /* ssize_t, etc */
#include <fcntl.h>      /* Defines O_* constants */
#include <sys/stat.h>   /* Defines mode constants */
#include <semaphore.h>  /* POSIX semaphore API */
#include <sys/mman.h>   /* POSIX shared memory API */
#include <unistd.h>     /* read(), write(), STDIN_FILENO, STDOUT_FILENO, close() */
#include <stdio.h>
#include <stdlib.h>

/* Hard-coded POSIX IPC object names */

#define XFR_WRITE_SEM "/xfr_write_psem"
#define XFR_READ_SEM "/xfr_read_psem"
#define XFR_PSHM "/xfr_pshm"

#define OBJ_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)
                                /* Permissions for our POSIX IPC objects */

#ifndef BUF_SIZE                /* Allow "cc -D" to override definition */
#define BUF_SIZE 1024           /* Size of transfer buffer */
#endif

struct pshmobj {                /* Defines structure of shared memory segment */
    ssize_t cnt;                /* Number of bytes used in 'buf' */
    char buf[BUF_SIZE];         /* Data being transferred */
};

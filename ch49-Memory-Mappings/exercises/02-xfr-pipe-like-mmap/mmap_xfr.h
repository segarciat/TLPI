/* Modified by Sergio E. Garcia Tapia for Exercise 49-2 */

/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2023.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Exercise 49-2 (based on Listing 48-1) */

/*  mmap_xfr.h

   Header file used by the mmap_xfr_reader.c and mmap_xfr_writer.c programs.
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "binary_sems.h"        /* Declares our binary semaphore functions */
#include "tlpi_hdr.h"

/* Hard-coded keys for IPC objects */

#define MMAP_XFR_SHARED_FILE "mmap_xfr_shared_file"		/* Key for shared memory segment */
#define SEM_KEY 0x5678									/* Key for semaphore set */

#define OBJ_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)
                                /* Permissions for our System V semaphore set */

/* Two semaphores are used to ensure exclusive, alternating access
   to the shared memory segment */

#define WRITE_SEM 0             /* Writer has access to shared file mapping */
#define READ_SEM 1              /* Reader has access to shared file mapping */

#ifndef BUF_SIZE                /* Allow "cc -D" to override definition */
#define BUF_SIZE 1024           /* Size of transfer buffer */
#endif

struct mmap_region {            /* Defines structure of shared file mapping */
    int cnt;                    /* Number of bytes used in 'buf' */
    char buf[BUF_SIZE];         /* Data being transferred */
};

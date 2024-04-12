# Exercise 15-3

On a system running Linux 2.6, modify the program Listing 15-1 (`t_stat.c`) so that the file
timestamps are displayed with nanosecond accuracy.

## Solution

To obtain the nanosecond timestamp accuracy, we need to use the appropriate feature test macro:

```c
/* See page 289, section 15.2.2 */
#include _XOPEN_SOURCE 700	/* Or define _POSIX_C_SOURCE >= 200809 */
#include <sys/stat.h>
```

Then, as mentioned in section 15.2 (page 287), the seconds component will be accessible
through the traditional fields (`st_atime`, `st_mtime`, and `st_ctime`). Additionally,
the nanosecond components can be accessed using `st_atim.tv_nsec`, for example. Note it is
`atim`, and not `atime`.

I modified `t_stat.c` by adding the following function:

```c
* Display times with nanosecond precision */
static void
displayTime(char *format, struct timespec t)
{
    /* Break seconds into time components */
    struct tm *tParts = localtime(&t.tv_sec);
    if (tParts == NULL)
        errExit("localtime");

    /* Create custom time format string with nano seconds */
    char nanosecFmt[BUFSIZ];
    char timestr[BUFSIZ];
    snprintf(nanosecFmt, BUFSIZ, NANOSEC_FMT, (long long) t.tv_nsec);
    strftime(timestr, BUFSIZ, nanosecFmt, tParts);

    /* Display the time */
    printf(format, timestr);
}
```

Then I replaced the old `printf` calls that made use of `ctime` with a call to this function:

```c
/*  
    printf("Last file access:         %s", ctime(&sb->st_atime));
    printf("Last file modification:   %s", ctime(&sb->st_mtime));
    printf("Last status change:       %s", ctime(&sb->st_ctime));
    */

    displayTime("Last file access:         %s\n", sb->st_atim);
    displayTime("Last file modification:   %s\n", sb->st_mtim);
    displayTime("Last status change:       %s\n", sb->st_ctim);
```

## Sample Run

The `makefile` assumes the presence of `tlpi_dist` at the root of the repository, which contains
`libtlpi.a`:

```bash
make
./t_stat t_stat.c
```

Sample output:

```
File type:                regular file
Device containing i-node: major=259   minor=5
I-node number:            6194707
Mode:                     100644 (rw-r--r--)
Number of (hard) links:   1
Ownership:                UID=1000   GID=1000
File size:                4968 bytes
Optimal I/O block size:   4096 bytes
512B blocks allocated:    16
Last file access:         Fri Apr 12 15:57:18.238555733 2024
Last file modification:   Fri Apr 12 15:57:17.574555281 2024
Last status change:       Fri Apr 12 15:57:17.574555281 2024
```

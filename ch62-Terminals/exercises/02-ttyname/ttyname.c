#include "ttyname.h"
#include <errno.h>      /* errno, EINVAL */
#include <sys/stat.h>   /* struct stat */
#include <unistd.h>     /* fstat(), lstat() */
#include <stddef.h>     /* NULL */
#include <dirent.h>     /* opendir(), struct dirent, close() */
#include <string.h>     /* strstr() */
#include <stdio.h>      /* snprintf() */

/* Returns 1 if found, 0 if did not find, and -1 on error */
static int
_findtty(const char *dirpath, char *pattern, struct stat* filesbp, char *ttybuf, size_t bufsize)
{
    DIR *dirp = opendir(dirpath);
    if (dirp == NULL)
        return -1;

    /* Error or truncated */
    int dirpathLen = snprintf(ttybuf, bufsize, "%s", dirpath);
    if (dirpathLen <= 0 || dirpathLen >= bufsize)
        return -1;

    struct dirent* entp;
    int found = 0;
    for (;;) {
        /* Error or EOF */
        errno = 0;
        entp = readdir(dirp);
        if (entp == NULL)
            break;

        /* Match names beginning with tty */
        if (pattern != NULL && strstr(entp->d_name, pattern) == NULL)
            continue;
        /* Must match rdev */
        struct stat devsb;
        snprintf(ttybuf + dirpathLen, bufsize - dirpathLen, "%s", entp->d_name);
        if (lstat(ttybuf, &devsb) != -1 && filesbp->st_rdev == devsb.st_rdev){
            found = 1;
            break;
        }
    }
    closedir(dirp);
    if (errno != 0)
        return -1;
    return found;
}

int
_ttyname(int fd, char *ttybuf, size_t bufsize)
{
    if(ttybuf == NULL || bufsize < TTYNAME_MAX) {
        errno = EINVAL;
        return -1;
    }

    struct stat filesb;

    if (fstat(fd, &filesb) == -1)
        return -1;
    if (_findtty("/dev/", "tty", &filesb, ttybuf, bufsize) <= 0 &&
        _findtty("/dev/pts/", NULL, &filesb, ttybuf, bufsize) <= 0)
        return -1;
    return 0;
}

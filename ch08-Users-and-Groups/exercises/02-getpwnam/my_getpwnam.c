#define _DEFAULT_SOURCE /* Get definition of getpwent(), setpwent(), endpwent() */

#include <string.h> // strncmp
#include <unistd.h> // sysconf
#include <stddef.h> // NULL
#include "my_getpwnam.h"

#define MAXLN_DEFAULT 256


/* my_getpwnam: Given login `name`, returns a pointer to the associated passwd structure
 * To be SUSv3 comformant, whenever a matching `passwd` record cannot be found, it should
 * return NULL and leave errno unchanged.
 */
struct passwd*
my_getpwnam(const char *name)
{
    if (name == NULL || *name == '\0')
        return NULL;
    
    /* Open password file and rewind to beginning */
    setpwent();

    long lnmax = sysconf(_SC_LOGIN_NAME_MAX); /* includes null byte */
    if (lnmax == -1)
        lnmax = MAXLN_DEFAULT; /* guess */

    struct passwd *p;
    while((p = getpwent()) != NULL && strncmp(name, p->pw_name, (size_t) lnmax) != 0)
        ;

    /* Close password file */
    endpwent();

    return p;
}

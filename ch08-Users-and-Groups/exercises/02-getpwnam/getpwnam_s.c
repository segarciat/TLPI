#include <string.h> // strncmp
#include <unistd.h> // sysconf
#include "getpwnam_s.h"
#include <stdio.h>


/* getpwnam_s: Given login `name`, returns a pointer to the associated passwd structure
 * To be SUSv3 comformant, whenever a matching `passwd` record cannot be found, it should
 * return NULL and leave errno unchanged.
 */
struct passwd
*getpwnam_s(const char *name)
{
	if (name == NULL || *name == '\0') /* NULL string or empty */
		return NULL;
	
	/* Open the password file and get pointer to start of list */
	struct passwd *p = getpwent();

	/* restart from te beginning if not already there */
	setpwent();


	long lnmax = sysconf(_SC_LOGIN_NAME_MAX);
#define MAXLN_DEFAULT 256
	if (lnmax == -1)
		lnmax = MAXLN_DEFAULT;

	while(p != NULL && strncmp(name, p->pw_name, lnmax) != 0)
		p = getpwent();

	/* Close password file */
	endpwent();

	return p;
}

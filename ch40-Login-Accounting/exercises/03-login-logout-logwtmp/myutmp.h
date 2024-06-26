#ifndef _H_MYUTMP_HEADER
#define _H_MYTUMP_HEADER
#include <utmp.h>

void _login(const struct utmp *ut);
int _logout(const char *ut_line);
void _logwtmp(const char *line, const char *name, const char *host);
#endif

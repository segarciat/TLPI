#ifndef MY_GETPWNAM
#define MY_GETPWNAM

#include <pwd.h>    // getpwent, setpwent, endpwent, struct passwd

struct passwd *my_getpwnam(const char *name);

#endif

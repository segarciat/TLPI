#include <pwd.h>    // getpwent, setpwent, endpwent, struct passwd

struct passwd
*getpwnam_s(const char *name);


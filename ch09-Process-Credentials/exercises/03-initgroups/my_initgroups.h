#ifndef MY_INITGROUPS_H
#define MY_INITGROUPS_H

#include <sys/types.h> // gid_t

int my_initgroups(const char *user, gid_t group);

#endif

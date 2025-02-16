#ifndef SET_UNSET_ENV_H
#define SET_UNSET_ENV_H

/*
 * my_set_env:
 *
 * Set the environment name-value pair name=value.
 * If the variable already exists, change its value if overwrite is nonzero.
 * Otherwise, do nothing.
 *
 * Returns 0 on success, or -1 on error.
 */
int my_setenv(const char *name, const char *value, int overwrite);

/*
 * my_unsetenv:
 *
 * remove environment variable of given name
 *
 * Returns 0 on success, -1 on error.
 */
int my_unsetenv(const char *name);

#endif

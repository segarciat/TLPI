/* set the environment name-value pair name=value.
 * If the variable already exists, change its value if overwrite is nonzero.
 * Otherwise, do nothing.
 *
 * Returns 0 on success, or -1 on error.
 */
int setenv_s(const char *name, const char *value, int overwrite);

/* remove environment variable of given name
 *
 * Returns 0 on success, -1 on error.
 */
int unsetenv_s(const char *name);

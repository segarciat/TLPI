#include <stdio.h>

/*
 * Execs a shell to execute the given command, and returns a stdio stream. If "r" is
 * specified in mode, then the caller can read from the stream. If "w" is specified,
 * then the caller can write to the stream. Otherwise, NULL is returned and errno is
 * set to EINVAL.
 * On error, returns NULL. Possible error causes include failing to allocate memory,
 * failing to close file descriptors, failing to fork, or failing to exec a shell.
 */
FILE *_popen(const char *command, const char *mode);

/*
 * Uses the given stream, which shoudl have been obtained by an earlier called to
 * _popen(), to close the pipe that was opened at that time. The child shell
 * execed to run the command specified in _popen() is reaped. If a shell could
 * not be execed, returns as though the child shell terminated with _exit(127).
 * On any other error, returns -1.
 */
int _pclose(FILE *stream);

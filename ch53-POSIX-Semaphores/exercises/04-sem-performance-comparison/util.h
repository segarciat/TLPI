#include <stdbool.h>    /* bool */

#define USAGE "Usage: %s sem-name count\n" \
                "Increment and decrement the value of the semaphore the number of times specified\n"

/* Displays given message on stderr, and optionally, the message associated with the current value of errno.
   Then, exit
*/
void errorExit(bool showErrno, const char* fmt, ...);

/* Parses a string as an integer, and places the result in the buffer n.
   Returns 0 if successful, or -1 if an error or invalid counter is encountered
*/
int parseLong(const char *s, long* n);

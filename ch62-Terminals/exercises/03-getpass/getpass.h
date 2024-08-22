#ifndef _GETPASS_H
#define _GETPASS_H

#include <stddef.h>

#define PASS_MAX BUFSIZ

/* Prints the given prompt string and disables echoing and processing of all terminal characters
   (except EOF), including terminal generated signals. Then reads user input until an error, EOF,
   a newline, or PASS_MAX characters are encountered. The given input is returned, with the trailing
   newline character stripped. The input passbuf must be a buffer with bufsize characters, and
   bufsize must be at least PASS_MAX (or it fails with EINVAL). Other errors may occur from associated
   system calls used. Upon completion, returns terminal settings that were in effect at the start of
   the call. On success, returns passbuf, and on error, returns NULL.
*/
char* _getpass(const char *prompt, char *passbuf, size_t bufsize);

#endif

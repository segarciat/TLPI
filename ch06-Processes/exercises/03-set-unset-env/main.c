#define _DEFAULT_SOURCE /* Expose clearenv() in stdlib.h */

#include <stdio.h>
#include <stdlib.h>
#include "set_unset_env.h"

extern char** environ;

int
main()
{
	clearenv(); // Erase environment

	my_setenv("GREETING", "HELLO", 0);
	my_setenv("GREETING", "HI", 0);
	my_setenv("GREETING", "Hiya", 1);
	my_setenv("EXERCISE", "JUMP", 0);
	my_setenv("BEVERAGE", "WATER", 0);
	my_unsetenv("EXERCISE");

	for (char **p = environ; *p != NULL; p++)
		puts(*p);
	exit(EXIT_SUCCESS);
}

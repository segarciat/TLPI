#include <stdio.h>
#include <stdlib.h>
#include "set_unset_env.h"

extern char** environ;

int
main(int argc, char *argv[])
{
	clearenv(); // Erase environment

	setenv_s("GREETING", "HELLO", 0);
	setenv_s("GREETING", "HI", 0);
	setenv_s("GREETING", "Hiya", 1);
	setenv_s("EXERCISE", "JUMP", 0);
	setenv_s("BEVERAGE", "WATER", 0);
	unsetenv_s("EXERCISE");

	for (char **p = environ; *p != NULL; p++)
		puts(*p);
	exit(EXIT_SUCCESS);
}

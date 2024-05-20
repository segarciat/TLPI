#include "s_execlp.h"
#include <stdio.h> /* fprintf(), stderr, NULL */
#include <unistd.h> /* execlp */
#include <string.h>

extern char **environ;

int
main(int argc, char *argv[])
{
	s_execlp("ls", "-l", "-a", (char *) NULL);
	fprintf(stderr, "Failed\n");

	/*
	s_execlp("./myscript.sh", (char *) NULL);
	fprintf(stderr, "Failed\n");
	*/


	/*
	execlp(NULL, (char *) NULL);
	fprintf(stderr, "Failed execlp\n");
	*/

	/*
	char *s_argv[] = { "/bin/sh", "./myscript.sh", NULL };
	execve("/bin/sh", s_argv, environ);
	*/
}

#include <unistd.h> /* execve */
#include <stdio.h> /* fprintf(), stderr, NULL */
#include <stdlib.h> /* exit(), EXIT_FAILURE */
#define PROGRAM "./test.cat"

extern char **environ;

int
main(int argc, char *argv[])
{
	char *args[] = { PROGRAM, NULL };
	execve(PROGRAM, args, environ);
	fprintf(stderr, "%s: Failed to exec %s\n", argv[0], PROGRAM);
	exit(EXIT_FAILURE);
}

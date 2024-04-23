#include <limits.h> /* PATH_MAX */
#include <stdio.h> /* stderr, fprintf(), printf() */
#include <stdlib.h> /* exit(), EXIT_SUCCESS, EXIT_FAILURE */
#include "getcwd.h"

int
main(int argc, char *argv[])
{
	char cwd[PATH_MAX];
	if (_getcwd(cwd, PATH_MAX) == NULL) {
		fprintf(stderr, "Failed to get current working directory\n");
		exit(EXIT_FAILURE);
	}
	printf("%s\n", cwd);
	exit(EXIT_SUCCESS);
}

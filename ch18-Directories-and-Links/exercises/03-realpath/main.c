#include <stdio.h>	/* stderr, fprintf(), puts(), NULL */
#include <stdlib.h> /* exit(), EXIT_FAILURE, EXIT_SUCCESS */
#include <limits.h>	/* PATH_MAX */
#include "s_realpath.h"

int
main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s path\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	char resolved[PATH_MAX];
	if (_realpath(argv[1], resolved) == NULL) {
		fprintf(stderr, "Bad path\n");
		exit(EXIT_FAILURE);
	}
	puts(resolved);
	exit(EXIT_SUCCESS);
}

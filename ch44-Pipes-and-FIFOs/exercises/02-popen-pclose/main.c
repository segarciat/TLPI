#include "popen_pclose.h" /* _popen(), _pclose() */
#include <stdio.h> /* fprintf(), printf(), FILE, stderr, stdout, BUFSIZ, fgets(), fputs() */
#include <stddef.h> /* NULL */
#include <stdlib.h> /* exit(), EXIT_FAILURE, EXIT_SUCCESS */
#include <string.h> /* strerror() */
#include <errno.h> /* errno */

#define WRITE_COMMAND "/bin/ls"
#define READ_COMMAND "cat"

int
main(int argc, char *argv[])
{
	FILE *fp;
	char line[BUFSIZ];

	/* Open pipe in read mode */
	fp = _popen(WRITE_COMMAND, "r");
	if (fp == NULL) {
		fprintf(stderr, "%s: Command %s failed: %s\n", argv[0], WRITE_COMMAND, strerror(errno));
		exit(EXIT_FAILURE);
	}

	while (fgets(line, BUFSIZ, fp) != NULL)
		fputs(line, stdout);
	
	if (_pclose(fp) == -1) {
		fprintf(stderr, "%s: _pclose failed: %s\n", argv[0], strerror(errno));
		exit(EXIT_FAILURE);
	}

	/* Open pipe in write mode */
	fp = _popen(READ_COMMAND, "w");
	if (fp == NULL) {
		fprintf(stderr, "%s: Command %s failed: %s\n", argv[0], READ_COMMAND, strerror(errno));
		exit(EXIT_FAILURE);
	}

	setbuf(fp, NULL);
	fprintf(fp, "Child should print this out\n");
	if (_pclose(fp) == -1) {
		fprintf(stderr, "%s: _pclose failed: %s\n", argv[0], strerror(errno));
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}

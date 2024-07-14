#include "ftok.h" /* _ftok() */
#include <sys/types.h> /* key_t */
#include <sys/ipc.h> /* ftok() */
#include <string.h> /* strcmp(), strerror() */
#include <stdio.h> /* fprintf(), printf() */
#include <stdlib.h> /* exit(), EXIT_FAILURE, EXIT_SUCCESS, srand(), rand() */
#include <time.h> /* time() */
#include <stddef.h> /* NULL */
#include <errno.h> /* errno */


#define USAGE_FORMAT "%s [-h | --help] filename\n"

int
main(int argc, char *argv[])
{
	key_t realFtokKey, myFtokKey;
	int proj;
	char *filename;

	if (argc > 1 && (strcmp("-h", argv[1]) == 0 || strcmp("--help", argv[1]) == 0)) {
		printf(USAGE_FORMAT, argv[0]);
		exit(EXIT_SUCCESS);
	}
	if (argc != 2) {
		fprintf(stderr, USAGE_FORMAT, argv[0]);
		exit(EXIT_FAILURE);
	}
	filename = argv[1];

	/* Generate proj */
	srand(time(NULL));
	proj = rand() % 256 + 1;
	printf("Generated proj value: %d\n", proj);

	/* Compute real ftok() */
	realFtokKey = ftok(filename, proj);
	if (realFtokKey == -1) {
		fprintf(stderr, "%s: Real ftok() failed: %s\n", argv[0], strerror(errno));
		exit(EXIT_FAILURE);
	}

	/* Compute my _ftok() */
	myFtokKey = _ftok(filename, proj);
	if (myFtokKey == -1) {
		fprintf(stderr, "%s: Custom _ftok() failed: %s\n", argv[0], strerror(errno));
		exit(EXIT_FAILURE);
	}

	printf("ftok() : %d\n_ftok(): %d\n", realFtokKey, myFtokKey);
	exit(EXIT_FAILURE);
}

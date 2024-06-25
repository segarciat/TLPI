#include "getlogin.h"
#include <stddef.h> /* NULL */
#include <stdio.h> /* fprintf(), printf(), stderr */
#include <stdlib.h> /* exit(), EXIT_FAILURE, EXIT_SUCCESS */
#include <string.h>
#include <unistd.h> /* getlogin() */
#include <errno.h>

int
main(int argc, char *argv[])
{
	char *loginName = getlogin_s();
	if (loginName == NULL) {
		fprintf(stderr, "%s: Failed to get login name (%s)\n", argv[0], strerror(errno));
		loginName = getlogin();
		if (loginName == NULL) {
			fprintf(stderr, "%s: glibc getlogin() also failed (%s)\n", argv[0], strerror(errno));
		} else {
			fprintf(stderr, "%s: However, glibc getlogin() was successful: %s\n", argv[0], loginName);
		}
		exit(EXIT_FAILURE);
	}
	printf("Login name: %s\n", loginName);
	exit(EXIT_SUCCESS);
}

#include <string.h>
#include <stdio.h>
#include <unistd.h> // sysconf
#include <stdlib.h> // exit, EXIT_FAILURE
#include "getpwnam_s.h"

int
main(int argc, char *argv[])
{
	long lnmax = sysconf(_SC_LOGIN_NAME_MAX);
	if (lnmax == -1)
		lnmax = 256;
	char *username = malloc(lnmax);
	if (username == NULL) {
		fprintf(stderr, "malloc failed\n");
		exit(EXIT_FAILURE);
	}
	printf("Username: ");
	if (fgets(username, lnmax, stdin) == NULL)
		exit(EXIT_FAILURE);
	size_t len = strlen(username);
	if (username[len-1] == '\n') // Remove trailing newline character {
		username[len-1] = '\0';
	
	struct passwd* pwrecord =  getpwnam_s(username);
	if (pwrecord == NULL) {
		fprintf(stderr, "Failed to get record for %s\n", username);
		exit(EXIT_FAILURE);
	}

	printf("Found: %s with UID %ld\n", pwrecord->pw_name, (long) pwrecord->pw_uid);
	exit(EXIT_SUCCESS);
}

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
void
unix_error(char *msg)
{
	fprintf(stderr, "%s: %s\n", msg, strerror(errno));
	exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
	char buf[BUFSIZ];
	pid_t childPid;
	switch(childPid = fork()) {
		case -1: unix_error("Fork failed");
		case  0: /* Child */
			printf("Child going to attempt to read input\n");
			ssize_t bytesRead = read(STDIN_FILENO, buf, BUFSIZ); 
			if (bytesRead == -1 && errno != EIO)
				unix_error("Read failed in child, but it wasn't EIO was expected");
			else if (bytesRead != -1) {
				fprintf(stderr, "Child was able to read %ld bytes, which should not have worked\n"
					"Be sure to run program in background using & (ampersand)\n", (long) bytesRead);
				exit(EXIT_FAILURE);
			}
			printf("As expected, program ended due to EIO: %s\n", strerror(errno));
			exit(EXIT_SUCCESS);
		default: /* Parent */
			printf("Parent exiting\n");
			_exit(EXIT_SUCCESS);
	}
}

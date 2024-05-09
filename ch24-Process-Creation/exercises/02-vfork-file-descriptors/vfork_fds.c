#include <unistd.h> /* vfork() */
#include <stdlib.h> /* exit(), EXIT_SUCCESS, EXIT_FAILURE */
#include <unistd.h> /* write(), read(), close() */

#define BUF_SIZE 4096

int
main(int argc, char *argv[])
{
	pid_t childPid;
	/* fork */
	childPid = vfork();
	switch(childPid) {
		case -1: /* error */
			write(STDERR_FILENO, "vfork failed\n", 14);
			exit(EXIT_FAILURE);
		case 0: /* child */
			if (close(STDIN_FILENO) == -1) {
				/* Display some error message */
				write(STDERR_FILENO, "Error in child\n", 16);
				_exit(EXIT_FAILURE);
			}
			_exit(EXIT_SUCCESS);
		default:
			/* Try operating with file descriptor */
			if (write(STDOUT_FILENO, "Provide some input: ", 21) == -1) {
				write(STDERR_FILENO, "Error in parent\n", 17);
				exit(EXIT_FAILURE);
			}
			char buf[BUF_SIZE];
			ssize_t numRead;
			if ((numRead = read(STDIN_FILENO, buf, BUF_SIZE)) == -1) {
				write(STDERR_FILENO, "Error reading in parent", 24);
				exit(EXIT_FAILURE);
			}

			if (write(STDOUT_FILENO, buf, numRead) != numRead) {
				write(STDERR_FILENO, "Error echoing from parent", 26);
				exit(EXIT_FAILURE);
			}
			exit(EXIT_SUCCESS);
			}
}

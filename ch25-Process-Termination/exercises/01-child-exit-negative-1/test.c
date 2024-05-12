#include <unistd.h> /* _exit(), fork(), wait()  */
#include <stdio.h> /* printf(), fprintf() */
#include <stdlib.h> /* exit(), EXIT_SUCCESS */
#include <sys/wait.h> /* wait() */

int
main(int argc, char *argv[])
{
	pid_t childPid;
	int status;
	switch(childPid = fork()) {
		case -1:
			fprintf(stderr, "Failed to work()");
			exit(EXIT_FAILURE);
		case 0:
			printf("Child exiting\n");
			_exit(-2);
		default:
			if (wait(&status) == -1) {
				fprintf(stderr, "Parent received -1 from child\nn");
				exit(EXIT_FAILURE);
			}
			printf("Parent got the following exit status from child: %x\n", status);
			exit(EXIT_SUCCESS);
	}
}

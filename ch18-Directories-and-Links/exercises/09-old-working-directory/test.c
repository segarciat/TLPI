#include <unistd.h>	/* chdir(), fchdir(), getcwd() */
#include <limits.h> /* PATH_MAX */
#include <fcntl.h>	/* open(), close() */
#include <stdio.h> /* fprintf(), stderr */
#include <stdlib.h> /* exit(), EXIT_FAILURE, EXIT_SUCCESS */
#include <time.h>

void using_open(long runs)
{
	int fd;

	fd = open(".", O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, "open filed on .\n");
		exit(EXIT_FAILURE);
	}
	for (long i = 0; i < runs; i++) {
		chdir("..");
		fchdir(fd);
	}

	close(fd);
}

void using_getcwd(long runs)
{
	char buf[PATH_MAX];

	getcwd(buf, PATH_MAX);
	for (long i = 0; i < runs; i++) {
		chdir("..");
		chdir(buf);
	}
}

static void timeApproach(void (*f)(long runs), long runs, char *msg)
{
	clock_t start, end;
	if ((start = clock()) == -1) {
		fprintf(stderr, "Could not get start time\n");
		exit(EXIT_FAILURE);
	}
	f(runs);
	if ((end = clock()) == -1) {
		fprintf(stderr, "Could not get end time\n");
		exit(EXIT_FAILURE);
	}
	printf("%s: %.2f\n", msg == NULL ? "time" : msg, (double) (end - start) / CLOCKS_PER_SEC);
}

int
main(int argc, char *argv[])
{
	long runs = 1000000;
	timeApproach(using_open, runs, "fchdir() and open()");
	timeApproach(using_getcwd, runs, "chdir() and getcwd()");
}

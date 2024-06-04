#include <unistd.h> /* getopt(), optind, opterr, optopt, optarg */
#include <sys/resource.h> /* setpriority(), getpriority() */
#include <stdio.h> /* fprintf() */
#include <stdlib.h> /* exit(), EXIT_FAILURE */
#include <string.h> /* strerror() */
#include <errno.h> /* errono */

extern int optind, opterr, optopt;
extern char *optarg;

#define DEFAULT_NICE_INCREMENT 10

/* Display message related to errno and quit */
void
unix_error(char *msg)
{
	fprintf(stderr, "%s: %s\n", msg, strerror(errno));
	exit(EXIT_FAILURE);
}

void
usage(char *msg)
{
	printf("USAGE:\n"
			"\t nice\n"
			"\t\tDisplay nice value\n"
			"\t nice [-n N] COMMAND [ARGS...]\n"
			"\t\tRun COMMAND with given ARGS with niceness adjusted by N. Default is %d\n"
			"%s\n",
			DEFAULT_NICE_INCREMENT, msg);
}

/* 
 * Parse integer from string and return it.
 * If the string contains a non-numeric character or an error occurs, exits the program.
 */
int
parseInt(char *s)
{
	char *endp;
	long n;
	errno = 0;

	n = strtol(s, &endp ,0);
	if (errno != 0) {
		fprintf(stderr, "Integer parsing error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	} else if (*endp != '\0') {
		fprintf(stderr, "Non-numeric character '%c' in string %s\n", *endp, s);
		exit(EXIT_FAILURE);
	}

	return n;
}

int
main(int argc, char *argv[])
{
	int opt, nicenessIncr, priority, verbose, gotNice;

	nicenessIncr = DEFAULT_NICE_INCREMENT;
	verbose = 0;
	gotNice = 0;
	/* + to eliminate GNU permutation of arguments
	 * : (prefix) to ensure missing arguments report as : 
	 * : (after option letter) to enforce a required argument for option */
	while ((opt = getopt(argc, argv, "+:vn:")) != -1) {
		switch(opt) {
			case 'n':
				nicenessIncr = parseInt(optarg);
				gotNice = 1;
				break;
			case 'v':
				verbose = 1;
				break;
			case '?':
				fprintf(stderr, "Unrecognized option: (-%c)\n", optopt);
				exit(EXIT_FAILURE);
			case ':':
				fprintf(stderr, "Missing argument: (-%c)\n", optopt);
				exit(EXIT_FAILURE);
			default:
				fprintf(stderr, "Unexpected case reached while parsing arguments\n");
				exit(EXIT_FAILURE);
				break;
		}
	}
	/* Get current nice value */
	errno = 0;
	if ((priority = getpriority(PRIO_PROCESS,0)) == -1 && errno != 0)
		unix_error("Failed to get current nice value");
	
	if (argv[optind] == NULL) {
		if (gotNice) {
			usage("Must provide a command");
			exit(EXIT_FAILURE);
		} else {
			printf("%d\n", priority);
			exit(EXIT_SUCCESS);
		}
	}
	
	/* Display increment to use */
	if (verbose)

	/* Display value obtained */
	if (verbose) {
		printf("Nice value is currently: %d. Increasing by %d to %d\n", 
		priority, nicenessIncr, priority + nicenessIncr);
	}

	/* Set adjusted niceness vlaue */
	if (setpriority(PRIO_PROCESS, 0, priority + nicenessIncr) == -1)
		unix_error("Failed to adjust niceness");

	/* See actual niceness value, but don't fail if unsuccessful */
	if (verbose) {
		errno = 0;
		priority = getpriority(PRIO_PROCESS, 0);
		if (priority != -1 || errno == 0)
			printf("Nice value is now: %d. Gonna run: %s\n", priority, argv[optind]);
	}

	/* Execute given program */
	if (execvp(argv[optind], argv + optind) == -1) {
		fprintf(stderr, "Failed to run %s: %s\n", argv[optind], strerror(errno));
		exit(EXIT_FAILURE);
	}
}

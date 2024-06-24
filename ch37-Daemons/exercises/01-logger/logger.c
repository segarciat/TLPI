#include <syslog.h> /* LOG_USER */
#include <stdio.h> /* fprintf(), stderr */
#include <stdlib.h> /* exit(), EXIT_FAILURE */
#include <unistd.h> /* getopt(), optind, opterr, optopt, optarg */
#include <string.h> /* strcasecmp() */

#define DEFAULT_FACILITY LOG_USER
#define DEFAULT_LEVEL LOG_INFO

extern int optind, opterr, optopt;
extern char *optarg;

struct loglevel_s {
	const char *lvl_nm;
	int lvl_val;
};

static struct loglevel_s levels[] = {
	{.lvl_nm = "emerg",		.lvl_val = LOG_EMERG },
	{.lvl_nm = "alert",		.lvl_val = LOG_ALERT },
	{.lvl_nm = "crit",		.lvl_val = LOG_CRIT },
	{.lvl_nm = "err",		.lvl_val = LOG_ERR },
	{.lvl_nm = "warning",	.lvl_val = LOG_WARNING },
	{.lvl_nm = "notice", 	.lvl_val = LOG_NOTICE },
	{.lvl_nm = "info", 		.lvl_val = LOG_INFO },
	{.lvl_nm = "debug", 	.lvl_val = LOG_DEBUG },
};

#define N_LOG_LEVELS (sizeof(levels) / sizeof(levels[0]))

/* Returns the log level associated with the string, or -1 if an invalid string is provided. */
int
getLogLevel(char *s)
{
	for (int i = 0; i < N_LOG_LEVELS; i++)
		if (strcasecmp(levels[i].lvl_nm, s) == 0)
			return levels[i].lvl_val;
	return -1;
}

/* Display the available log levels on the specified stream */
void displayLogLevels(FILE *stream, char *prefix)
{
	int i;
	fprintf(stderr, "%s: ", prefix);
	for (i = 0; i < N_LOG_LEVELS - 1; i++)
		fprintf(stream, "%s,", levels[i].lvl_nm);
	fprintf(stream, "%s\n", levels[N_LOG_LEVELS-1].lvl_nm);
}

int
main(int argc, char *argv[])
{
	int opt, level;
	level = DEFAULT_LEVEL;
	while ((opt = getopt(argc, argv, ":l:")) != -1) {
		switch (opt) {
			case 'l':
				if ((level = getLogLevel(optarg)) == -1) {
					fprintf(stderr, "Invalid log level: %s\n", optarg);
					displayLogLevels(stderr, "Choose from");
					exit(EXIT_FAILURE);
				}
				break;
			case ':':
				fprintf(stderr, "Missing argument for: -%c\n", optopt);
				displayLogLevels(stderr, "Choose from");
				exit(EXIT_FAILURE);
			default:
				fprintf(stderr, "Unexpected case reached while parsing options\n");
				exit(EXIT_FAILURE);
		}
	}
	if (argc < optind) {
		fprintf(stderr, "Missing message argument\n");
		exit(EXIT_FAILURE);
	}
	syslog(DEFAULT_FACILITY | level, "%s", argv[optind]);
	exit(EXIT_SUCCESS);
}

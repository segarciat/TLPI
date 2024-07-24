#include "shmdir.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>
#include <stdarg.h>

#define SHOW_ERRNO 1

extern int optind, opterr, optopt;
extern char *optarg;

void
errorExit(int showErrno, char *fmt, ...)
{
	va_list varArgp;

	va_start(varArgp, fmt);
	if (fmt != NULL)
		vfprintf(stderr, fmt, varArgp);
	if (showErrno)
		fprintf(stderr, ": %s", strerror(errno));
	fprintf(stderr, "\n");
	va_end(varArgp);
	exit(EXIT_FAILURE);
}

void
usage(char *progName, FILE *stream)
{
	fprintf(stream, "Usage: %s [{-a|-u|-d|-g} -h] name [value]\n", progName);
	fprintf(stream, "    -h        View this help text.\n");
	fprintf(stream, "    -a        Add a name-value pair.\n");
	fprintf(stream, "    -u        Replace a value corresponding to given name with new specified value.\n");
	fprintf(stream, "    -g        Get value for given name.\n");
	fprintf(stream, "    -d        Delete name-value for given name.\n");
}

int
main(int argc, char *argv[])
{
	struct shmdir_ds ds;
	struct shmdir_ent ent;
	int opt, optionCount;
	int action;

	action = -1;
	optionCount = 0;

	/* 
	 * + to eliminate GNU permutation of arguments
	 * : (prefix) to ensure missing arguments report as : 
	 */
	while ((opt = getopt(argc, argv, "+:audgh")) != -1) {
		switch (opt) {
			case 'a':
			case 'u':
			case 'g':
			case 'd':
				action = opt;
				optionCount++;
				break;
			case 'h':
				usage(argv[0], stdout);
				exit(EXIT_SUCCESS);
			case '?':
				usage(argv[0], stderr);
				errorExit(!SHOW_ERRNO, "%s: Unrecognized option (-%c)", argv[0], optopt);
			default:
				usage(argv[0], stderr);
				errorExit(!SHOW_ERRNO, "%s: Unexpected case reached while parsing arguments", argv[0]);
		}
	}
	
	/* Validate options */
	if (optionCount != 1) {
		usage(argv[0], stderr);
		errorExit(!SHOW_ERRNO, "%s: Specify only one of {-a|-u|-g|-d}", argv[0]);
	}

	/* Ensure name argument was provided */
	if (argc < optind + 1 || argc > optind + 2) {
		usage(argv[0], stderr);
		errorExit(!SHOW_ERRNO, "%s: Incorrect number of arguments", argv[0]);
	}
	
	/* Initialize service */
	if (shmdir_init(&ds) == -1)
		errorExit(SHOW_ERRNO, "%s: Failed to initialize directory service: %s", argv[0]);
	// printf("%s: Successfully initialized directory service.\n", argv[0]);

	/* Carry out requested action */
	switch (action) {
		case 'a':
		case 'u':
			if (argc != optind + 2) {
				usage(argv[0], stderr);
				errorExit(!SHOW_ERRNO, "%s: Missing value argument", argv[0]);
			}
			strncpy(ent.nam, argv[optind], sizeof(ent.nam));
			printf("entry name: %s\n", ent.nam);
			strncpy(ent.val, argv[optind + 1], sizeof(ent.val));
			if (action == 'a') {
				if (shmdir_add(&ds, &ent) == -1)
					errorExit(SHOW_ERRNO, "%s: Error adding item: %s", argv[0], argv[optind]);
				printf("%s: Entry added successfully\n", argv[0]);
			} else {
				if (shmdir_upd(&ds, &ent) == -1)
					errorExit(SHOW_ERRNO, "%s: Error updating item: %s", argv[0], argv[optind]);
				printf("%s: Entry updated successfully\n", argv[0]);
			}
			break;
		case 'g':
			if (shmdir_get(&ds, argv[optind], &ent) == -1)
				errorExit(SHOW_ERRNO, "%s: Error getting item: %s", argv[0], argv[optind]);
			printf("%s\n", ent.val);
			break;
		case 'd':
			if (shmdir_del(&ds, argv[optind]) == -1)
				errorExit(SHOW_ERRNO, "%s: Error deleting item: %s", argv[0], argv[optind]);
			printf("%s: Succesfully deleted pair matching: %s\n", argv[0], argv[optind]);
			break;
		default:
			errorExit(!SHOW_ERRNO, "%s: Unexpected action: %c", argv[0], action);
	}

	/* Shutdown service */
	if (shmdir_shutdown(&ds) == -1)
		errorExit(SHOW_ERRNO, "%s: Failed to shutdown directory service: %s", argv[0]);
	// printf("%s: Successfully shut down directory service.\n", argv[0]);
	exit(EXIT_SUCCESS);
}

#include <stdio.h> /* fprintf(), stderr, printf(), BUFSIZ, fgets(), stdin, feof() */
#include <stdlib.h> /* exit(), EXIT_FAILURE */
#include <string.h> /* strerror(), strncasecmp(), strchr() */
#include <ctype.h> /* isspace() */
#include <errno.h> /* errno */
#include "bstree.h"

#define MAXLINE BUFSIZ
#define MAX_ACTION_PLUS_ARGS 5

/* Returns line read, or NULL on EOF */
char*
readLine(char *line, size_t size)
{
	/* Display user prompt */
	printf("\nChoose an action: \n\n"
		"\tlist          :    List all key-value pairs stored\n"
		"\tfind key      :    Find value with associated key.\n"
		"\tadd  key value:    Store pair (key, value) in collection.\n"
		"\tdel  key      :    Delete item with given key from collection\n"
		"\texit          :    Quit program\n\n"
		"> "
	);

	/* Get user input */
	char *p = fgets(line, size, stdin);
	if (p == NULL) {
		if (feof(stdin)) {		/* No input was given, clear EOF  */
			clearerr(stdin);
			return NULL;
		} else {				/* Error */
			fprintf(stderr, "Error reading input: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
	}
	return line;
}

/* Parse line into words (space-separated), storing up to size of them in given array */
size_t parseAction(char *line, char **actionPlusArgs, size_t size)
{
	char *p;
	/* Skip leading spaces and trailing newline, if any */
	size_t nargs = 0;
	while (nargs < size) {
		/* Skip leading blanks */
		while (isspace(*line)) line++;

		/* Store arg*/
		if ((p = strchr(line, ' ')) != NULL ||
			(p = strchr(line, '\t')) != NULL ||
			(p = strchr(line, '\n')) != NULL) {
			*p = '\0';
			actionPlusArgs[nargs++] = line;
			line = p + 1;
		} else
			break;
	}

	return nargs;
}

/* Display all key-value pairs in tree in ascending key order */
void
listtree(bstree_s *tree) {
	if (tree != NULL) {
		listtree(tree->left);
		if (tree->key != NULL)
			printf("%s: %s\n", tree->key, (char *) tree->val);
		listtree(tree->right);
	}
}

int
main(int argc, char *argv[])
{
	/* Enforce even number of arguments */
	if (argc < 2 || !(argc & 0x1)) {
		fprintf(stderr, "Usage: %s k_1 v_1 [k_n v_n]...\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* Create and initialize root of tree */
	bstree_s *tree;
	if (initialize(&tree) != 0) {
		fprintf(stderr, "Failed to initialize tree: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	/* Add given key-value pairs to tree */
	for (int i = 1; i < argc; i += 2) {
		char *key = argv[i];
		char *val = strdup(argv[i+1]);
		if (val == NULL) {
			fprintf(stderr, "Failed to copy value %s: %s\n", argv[i+1], strerror(errno));
			exit(EXIT_FAILURE);
		}
		char *oldVal = add(tree, key, val);
		if (oldVal == NULL) {
			fprintf(stderr, "Failed to add key-value pair (%s, %s): %s\n", key, val, strerror(errno));
			exit(EXIT_FAILURE);
		} else if (oldVal != val) /* In case a duplicate key is given */ {
			printf("Got duplicate key, removing old value\n");
			free(oldVal);
		}
	}

	char line[MAXLINE];
	char *actionPlusArgs[MAX_ACTION_PLUS_ARGS];
	char *p;
	while (1) {
		/* Display prompt and read input line */
		while ((p = readLine(line, MAXLINE)) == NULL)
			continue;

		/* Parse line into action and args */
		int nargs = parseAction(line, actionPlusArgs, MAX_ACTION_PLUS_ARGS);
		if (nargs == 0)
			continue;

		/* Determine action received */
		char *action = actionPlusArgs[0];
		printf("\n");
		if (strncasecmp("list", action, sizeof("list")) == 0) {
			listtree(tree);
		} else if (strncasecmp("find", action, sizeof("find")) == 0) {
			if (nargs < 2) {
				fprintf(stderr, "No search key provided\n");
				continue;
			}
			char *key = actionPlusArgs[1];
			char *val;
			if (lookup(tree, key, (void **) &val)) {
				printf("Found: %s\n", val);
			} else {
				printf("No match for key: %s\n", key);
			}
		} else if (strncasecmp("add", action, sizeof("add")) == 0) {
			if (nargs < 3) {
				fprintf(stderr, "Must provide key and value\n");
				continue;
			}
			char *key = actionPlusArgs[1];
			char *val = strdup(actionPlusArgs[2]);
			if (val == NULL) {
				fprintf(stderr, "Failed to create copy of given value %s: %s\n",
					actionPlusArgs[2], strerror(errno));
				continue;
			}
			char *oldVal = add(tree, key, val);
			if (oldVal == NULL) {
				fprintf(stderr, "Failed to add key-value pair (%s, %s): %s\n", key, val, strerror(errno));
				exit(EXIT_FAILURE);
			} else if (oldVal != val) {
				printf("Got duplicate key, removed old val: %s\n", oldVal);
				free(oldVal);
			}
			printf("Added: (%s, %s)\n", key, val);
		} else if (strncasecmp("del", action, sizeof("del")) == 0) {
			printf("Received del command\n");
		} else if (strncasecmp("exit", action, sizeof("exit")) == 0) {
			printf("Received exit command\n");
			break;
		} else {
			printf("Invalid action: %s\n", action);
			continue;
		}
	}

	exit(EXIT_SUCCESS);
}

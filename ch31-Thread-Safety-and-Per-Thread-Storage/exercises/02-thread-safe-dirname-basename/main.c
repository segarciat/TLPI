#include "paths.h"
#include <stdio.h>
#include <libgen.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>


void*
threadFunc(void *arg)
{
	char *path, *dirpath_ts, *dirpath_nr;

	path = strdup((char *) arg);
	dirpath_ts = dirname_ts(path);
	free(path);

	path = strdup((char *) arg);
	dirpath_nr = dirname_nr(path);
	free(path);

	printf("Peer thread (thread-safe  ) (%p): %s\n"  , dirpath_ts, dirpath_ts);
	printf("Peer thread (non-reentrant) (%p): %s\n\n", dirpath_nr, dirpath_nr);
	return NULL;
}

void
printPathParts(char *path, char *expectedDirname, char *expectedBasename)
{
	char *pdir, *pbase, *original, *actualDirname, *actualBasename;
	pbase = pdir = original = NULL;
	
	if (path != NULL)
		original = strdup(path);

	if (original != NULL)
		pdir = strdup(original);
	actualDirname = dirname_ts(pdir);
	if (actualDirname != pdir)
		free(pdir);

	if (original != NULL)
		pbase = strdup(original);
	actualBasename = basename_ts(pbase);
	if (actualBasename != pbase)
		free(pbase);

	printf("(%s): dirname: %s -> %s\n",
		strcmp(actualDirname, expectedDirname) == 0 ? "pass" : "fail",
		original, actualDirname);

	printf("(%s): basename: %s -> %s\n",
		strcmp(actualBasename, expectedBasename) == 0 ? "pass": "fail",
		original, actualBasename);

	/* Clean up */
	if (actualDirname == pdir)
		free(pdir);
	if (actualBasename == pbase)
		free(pbase);
	
}

int
main(int arg, char *argv[])
{
	int s;
	pthread_t tid;
	char *path, *dirpath_ts, *dirpath_nr;

	/* Sanity check with thread-safe version */
	printPathParts("/", "/", "/");
	printPathParts("/usr/bin/zip", "/usr/bin", "zip");
	printPathParts("/etc/passwd////", "/etc", "passwd");
	printPathParts("/etc////passwd", "/etc", "passwd");
	printPathParts("etc/passwd", "etc", "passwd");
	printPathParts("passwd", ".", "passwd");
	printPathParts("passwd/", ".", "passwd");
	printPathParts("..", ".", "..");
	printPathParts(NULL, ".", ".");

	/* Store result of thread-safe version */
	path = strdup("/usr/bin/zip");
	dirpath_ts = dirname_ts(path);
	free(path);

	/* Store result of non-reentrant version and display */
	path = strdup("/usr/bin/zip");
	dirpath_nr = dirname_nr(path);
	printf("\nMain thread (non-reentrant) (%p): %s\n\n", dirpath_nr, dirpath_nr);

	/* Create peer thread and wait for it to finish */
	s = pthread_create(&tid, NULL, threadFunc, "/etc/passwd////");
	if (s != 0) {
		fprintf(stderr, "Failed to create thread: %s\n", strerror(s));
		exit(EXIT_FAILURE);
	}
	s = pthread_join(tid, NULL);
	if (s != 0) {
		fprintf(stderr, "Failed to join thread: %s\n", strerror(s));
		exit(EXIT_FAILURE);
	}

	/* Display both results (thread-safe and non-reentrant version) after peer thread finishes */
	printf("\nMain thread (thread-safe  ) (%p): %s\n", dirpath_ts, dirpath_ts);
	printf("Main thread (non-reentrant) (%p): %s\n", dirpath_nr, dirpath_nr);
	pthread_exit((void *) EXIT_SUCCESS);
}

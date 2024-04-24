#define _XOPEN_SOURCE 500
#include <ftw.h> /* nftw(), struct FTW */
#include "tlpi_hdr.h"

#include <stdio.h> /* printf(), fprintf(), perror() */
#include <string.h> /* strcmp() */
#include <stdlib.h> /* exit(), EXIT_SUCCESS, EXIT_FAILURE */
#include <sys/inotify.h> /* inotifiy_init(), inotify_add_watch(), struct inotify_event */
#include <limits.h> /* NAME_MAX */


static int g_inotifyFd;
struct watchlist_s {
	int wd;
	struct watchlist_s *next;
	char pathname[PATH_MAX];
};
static struct watchlist_s *g_head = NULL;

/* Add directory to watch list, allocating space for path string */
static void
watchNewDir(const char *pathname, size_t len)
{
	/* Watch lifecycle of files inside directory */
	int flags = IN_CREATE | IN_DELETE | IN_MOVE | IN_ONLYDIR;
	int wd = inotify_add_watch(g_inotifyFd, pathname, flags);
	if (wd == -1)
		errExit("inotify_add_watch");

	/* Add to watchlist */
	struct watchlist_s *p = malloc(sizeof(struct watchlist_s));
	if (p == NULL)
		errExit("malloc");
	if (realpath(pathname, p->pathname) == NULL)
		errExit("realpath");
	p->next = g_head;
	p->wd = wd;
	g_head = p;
}

/* Remove directory from watchlist, deallocating the pathname */
static void
stopWatchingDir(const char *pathname)
{
	struct watchlist_s *previous, *cur;
	previous = NULL;
	cur = g_head;
	while (cur != NULL) {
		if (strncmp(cur->pathname, pathname, PATH_MAX) == 0)
			break;
		previous = cur;
		cur = cur->next;
	}
	if (cur == NULL)
		return;
	if (previous == NULL)
		g_head = cur->next;
	else
		previous = cur->next;
	if (inotify_rm_watch(g_inotifyFd, cur->wd) == -1)
		errExit("inotify_rm_watch");
	free(cur);
}

/* Finds node in watchlist, returning NULL if not found */
static struct watchlist_s*
watchlist_find(int wd)
{
	struct watchlist_s *cur = g_head;
	while(cur != NULL && cur->wd != wd)
		cur = cur->next;
	return cur;
}

/* Adds directory encountered by nftw to watch list */
static int
watchDirTree(const char *pathname, const struct stat *statbuf, int typeflag, struct FTW *ftwbuf)
{
	if (typeflag == FTW_NS) {
		fprintf(stderr, "Warning: Failed to stat directory: %s\nSkipping...\n", pathname);
	} else if (typeflag == FTW_DNR) {
		fprintf(stderr, "Warning: Cannot read directory: %s\nSkipping...\n", pathname);
	}
	else if (typeflag == FTW_D) {
		size_t len = strlen(pathname);
		watchNewDir(pathname, len);
	}
	return 0;
}

/* Logs message from event notification */
static void
logEvent(struct inotify_event *i)
{
	if (!(i->mask & (IN_CREATE | IN_DELETE | IN_MOVE)))
		return;
	if (i->len > 0)
		printf("%s: ", i->name);
	if (i->mask & IN_CREATE)
		printf("created in ");
	if (i->mask & IN_DELETE)
		printf("deleted from ");
	if (i->mask & IN_MOVED_FROM)
		printf("moved from ");
	if (i->mask & IN_MOVED_TO)
		printf("moved to ");
	struct watchlist_s *watchnode = watchlist_find(i->wd);
	if (watchnode != NULL)
		printf("%s", watchnode->pathname);
	else
		printf("wd=%d", i->wd);
	if (i->cookie > 0)
		printf(" (cookie = %d)", i->cookie);
	printf("\n");
}

#define DIR_ARG_IDX 1
#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

/* Logs all file creations, deletions, and renames under the directory
 * named in its command-line argument.
 */
int
main(int argc, char *argv[])
{
	if (argc != 2 || strcmp("--help", argv[1]) == 0)
		usageErr("%s dir\n", argv[0]);

	/* Initialize inotify instance */
	g_inotifyFd = inotify_init();
	if (g_inotifyFd == -1)
		errExit("inotify");
	
	/* Recursively watch lifecycle of files in provided directory */
	int nopenfd = 20;
	int flags = FTW_PHYS; /* Do not dereference symbolic links */
	if (nftw(argv[DIR_ARG_IDX], watchDirTree, nopenfd, flags) == -1) {
		perror("Encountered error while recursively adding files to watch list with nftw()");
		exit(EXIT_FAILURE);
	}
	/* Log all file notifications received */
	char buf[BUF_LEN] __attribute__((aligned(8)));
	ssize_t numRead;
	for (;;) {
		/* Fetch pending events */
		numRead = read(g_inotifyFd, buf, BUF_LEN); /* Blocks until event occurs */
		if (numRead == 0)
			fatal("read() from inotify fd returned 0!");
		if (numRead == -1)
			errExit("read");

		/* Process events */
		for (char *p = buf; p < buf+ numRead; ) {
			struct inotify_event *event = (struct inotify_event *) p;
			logEvent(event);

			/* Handle created and deleted directories */

			if (event->mask & IN_ISDIR) {
				if (event->mask & IN_DELETE)
					stopWatchingDir(event->name);
				else if (event->mask & IN_CREATE)
					watchNewDir(event->name, event->len, 0);
			}
			p += sizeof(struct inotify_event) + event->len;
		}
	}
	exit(EXIT_SUCCESS);
}

#include "myutmp.h"
#include <unistd.h> /* getpid(), STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO */
#include <stddef.h> /* NULL */
#include <string.h> /* strncpy() */
#include <errno.h>
#include <paths.h> /* definitions of _PATH_UTMP and _PATH_WTMP */
#include <time.h>

void _login(const struct utmp *ut)
{
	/* Initialize to given entry */
	struct utmp ut_tolog = *ut;
	char *terminalName = NULL;

	/* Write entry as USER_PROCESS */
	ut_tolog.ut_type = USER_PROCESS;
	ut_tolog.ut_pid = getpid();

	/* Take pathname of first stream which is a terminal */
	if (isatty(STDIN_FILENO))
		terminalName = ttyname(STDIN_FILENO);
	else if (isatty(STDOUT_FILENO))
		terminalName = ttyname(STDOUT_FILENO);
	else if (isatty(STDERR_FILENO))
		terminalName = ttyname(STDERR_FILENO);

	/* Do not write anything */
	if (terminalName == NULL) {
		strncpy(ut_tolog.ut_line, "???", sizeof(ut_tolog.ut_line));
		return;
	}

	/* Skip the 5 characters /dev/ */
	strncpy(ut_tolog.ut_line, terminalName + 5, sizeof(ut_tolog.ut_line));
	strncpy(ut_tolog.ut_id, terminalName + 8, sizeof(ut_tolog.ut_id));

	/* Rewind to start of utmp file, and write records to utmp and wtmp */
	errno = 0;
	setutent();
	if (pututline(&ut_tolog) == NULL)
		return;
	updwtmp(_PATH_WTMP, &ut_tolog);

	/* Close file */
	endutent();
}

int _logout(const char *ut_line)
{
	struct utmp ut_query, *ut_matched;
	memset(&ut_query, 0, sizeof(struct utmp));
	strncpy(ut_query.ut_line, ut_line, sizeof(ut_query.ut_line));

	/* Find matching record */
	errno = 0;
	setutent();
	ut_matched = getutline(&ut_query);
	if (ut_matched == NULL)
		return 0;

	memset(ut_matched->ut_user, 0, sizeof(ut_matched->ut_user));
	memset(ut_matched->ut_host, 0, sizeof(ut_matched->ut_host));
	ut_matched->ut_tv.tv_sec = time(NULL);
	ut_matched->ut_type = DEAD_PROCESS;
	if (pututline(ut_matched) == NULL)
		return 0;
	return 1;
	
}

void _logwtmp(const char *line, const char *name, const char *host)
{
	struct utmp ut;

	memset(&ut, 0, sizeof(struct utmp));
	strncpy(ut.ut_line, line, sizeof(ut.ut_line));
	strncpy(ut.ut_user, name, sizeof(ut.ut_user));
	strncpy(ut.ut_host, host, sizeof(ut.ut_host));
	ut.ut_tv.tv_sec = time(NULL);
	ut.ut_pid = getpid();

	setutent();
	updwtmp(_PATH_WTMP, &ut);
}

#include <utmpx.h> /* struct utmpx, USER_PROCESS, setutxent(), getutxent(), endutxent() */
#include <stddef.h> /* NULL */
#include <unistd.h> /* ttyname(), STDIN_FILENO */
#include <string.h> /* memset() */
#include <errno.h> /* errno */

char*
getlogin_s(void)
{
	char *terminalName;
	struct utmpx ut, *user_ut;

	/* Get terminal name associated with process's standard input */
	if ((terminalName = ttyname(STDOUT_FILENO)) == NULL)
		return NULL;

	/* Clear structure and set search fields */
	memset(&ut, 0, sizeof(struct utmpx));
	ut.ut_type = USER_PROCESS;
	/* Assumes terminal whose name starts with /dev/ and skips those 5 characters */
	strncpy(ut.ut_line, terminalName + 5, sizeof(ut.ut_line));

	errno = 0;
	/* Open utmp file if not already open and rewind to start */
	setutxent();

	/* Search for matching entry, and close utmp file */
	user_ut = getutxline(&ut);
	endutxent();
	if (user_ut == NULL || errno != 0)
		return NULL;

	return user_ut->ut_user;
}

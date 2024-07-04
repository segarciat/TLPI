#include <unistd.h> /* pipe(), close(), read(), write() */
#include <stdio.h> /* fprintf(), vsnprintf(), BUFSIZ, fgets(), fflush(), stdout, stdin */
#include <stdlib.h> /* exit(), EXIT_FAILURE, EXIT_SUCCESS */
#include <string.h> /* strlen(), strerror() */
#include <errno.h> /* errno */
#include <stdarg.h> /* va_list, va_start(), va_end() */
#include <ctype.h> /* toupper() */
#include <sys/wait.h> /* wait() */

void
unix_error(char *format, ...)
{
#define BUF_SIZE 512
	char buf[BUF_SIZE];
	va_list argList;

	va_start(argList, format);
	vsnprintf(buf, BUF_SIZE, format, argList);
	fprintf(stderr, "%s: %s\n", buf, strerror(errno));
	va_end(argList);
	exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
	int uppercasePipeFds[2], unprocessedPipeFds[2];
	char text[BUFSIZ];
	ssize_t numRead;
	long textLen;

	/* Create pipes */
	if (pipe(unprocessedPipeFds) == -1 || pipe(uppercasePipeFds) == -1)
		unix_error("%s: Failed to create pipes", argv[0]);

	switch(fork()) {
		case -1:	/* Fork failed */
			unix_error("%s: Fork failed", argv[0]);
		case  0:	/* Child */
			if (close(unprocessedPipeFds[1]) == -1|| close(uppercasePipeFds[0]) == -1)
				unix_error("%s: Failed to close unused pipe descriptors in child");
			for (;;) {
				/* Read text from pipe */
				numRead = read(unprocessedPipeFds[0], text, BUFSIZ);
				if (numRead < 0)
					unix_error("%s: Child failed to read from pipe");
				else if (numRead == 0) /* EOF */
					break;

				/* Convert to uppercase */
				for (long i = 0; i < numRead; i++)
					text[i] = toupper(text[i]);

				/* Write data back to pipe */
				if (write(uppercasePipeFds[1], text, numRead) < 0)
					unix_error("%s: Child failed tow rite data to pipe");
			}
			if (close(unprocessedPipeFds[0]) == -1|| close(uppercasePipeFds[1]) == -1)
				unix_error("%s: Failed to close pipe descriptors in child");
			_exit(EXIT_SUCCESS);
		default:	/* Parent */
			/* Close unused FDs */
			if (close(unprocessedPipeFds[0]) == -1|| close(uppercasePipeFds[1]) == -1)
				unix_error("%s: Failed to close unused pipe descriptors in parent");

			for (;;) {
				/* Read data from stdin */
				fflush(stdout);
				if (fgets(text, BUFSIZ-1, stdin) == NULL)
					break;

				textLen = strlen(text);
				/* Write to pipe for child processing */
				if (write(unprocessedPipeFds[1], text, textLen) < 0)
					unix_error("%s: Parent failed to write to pipe", argv[0]);

				numRead = read(uppercasePipeFds[0], text, textLen);
				if (numRead < textLen)
					unix_error("%s: Parent did not received read enough characters from pipe", argv[0]);
				else if (numRead == 0)	/* EOF */
					break;
				text[numRead] = '\0';
				fputs(text, stdout);
			}
			/* Close pipe descriptors */
			if (close(unprocessedPipeFds[1]) == -1|| close(uppercasePipeFds[0]) == -1)
				unix_error("%s: Failed to close pipe descriptors in child");
			/* Reap child */
			wait(NULL);
			exit(EXIT_SUCCESS);
	}
}

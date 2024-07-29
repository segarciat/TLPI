#include <stdlib.h>		/* mkstmp(), exit(), EXIT_FAILURE, EXIT_SUCCESS */
#include <stdio.h>		/* fprintf(), vfprintf(), NULL, stderr */
#include <stdarg.h>		/* va_list, va_start(), va_end() */
#include <string.h>		/* strerror() */
#include <errno.h>		/* errno */
#include <stdbool.h>	/* bool, false */
#include <unistd.h>		/* unlink(), sysconf() */
#include <sys/mman.h>	/* mmap() */
#include <signal.h>		/* SIGSEGV, SIGBUS, struct sigset, struct sigaction, sigaction(), sigemptyset(), ... */
#include <setjmp.h>		/* sigjmp_buf, sigsetjmp, siglongjmp */

void
unix_error(bool showErrno, char *fmt, ...)
{
	va_list varArgp;

	va_start(varArgp, fmt);
	if (fmt != NULL)
		vfprintf(stderr, fmt, varArgp);
	if (showErrno)
		fprintf(stderr, "%s%s", fmt != NULL ? ": " : "", strerror(errno));
	fprintf(stderr, "\n");
	va_end(varArgp);
	exit(EXIT_FAILURE);
}

volatile sig_atomic_t sigBusCaught = 0;
volatile sig_atomic_t sigSegvCaught = 0;
volatile sig_atomic_t canJump = 0;
static sigjmp_buf senv;

void
sigHandler(int sig)
{
	if (!canJump)
		return;
	if (sig == SIGBUS)
		sigBusCaught = 1;
	else if (sig == SIGSEGV)
		sigSegvCaught = 1;
	siglongjmp(senv, 1);
}

int
main(int argc, char *argv[])
{
	char fileTemplate[] = "/tmp/tlpi-ex49-3XXXXXX";
	/* Create a temporary file */
	int fd = mkstemp(fileTemplate);
	if (fd == -1)
		unix_error(true, "%s: mkstemp() - Failed to create temporary file", argv[0]);
	if (unlink(fileTemplate) == -1)
		unix_error(true, "%s: unlink() - Failed to remove temporary file %s", argv[0], fileTemplate);

	/* Create a file mapping */
	long pageSize = sysconf(_SC_PAGESIZE);
	errno = 0;
	if (pageSize == -1)
		unix_error(errno != 0, "%s: mkstemp() - Failed to get page size", argv[0]);
	printf("%s: Page Size: %ld\n", argv[0], (long) pageSize);
	char *mmapAddr = mmap(NULL, pageSize, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	if (mmapAddr == MAP_FAILED)
		unix_error(true, "%s: mmap() - Failed to create private file mapping", argv[0]);

	/* Create SIGBUS and SIGSEGV handler */
	struct sigaction sa;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = sigHandler;
	if (sigaction(SIGSEGV, &sa, NULL) == -1)
		unix_error(true, "%s: sigaction() - Failed to install SIGSEGV handler", argv[0]);
	if (sigaction(SIGBUS, &sa, NULL) == -1)
		unix_error(true, "%s: sigaction() - Failed to install SIGBUS handler", argv[0]);

	/* Access, expect SIGBUS */
	if (sigsetjmp(senv, 1) == 0) {
		canJump = 1;
		printf("Attempting to memory region beyond the empty temporary file...\n");
		*mmapAddr = '1';
	}

	if (!sigSegvCaught) {
		printf("Attempting to write beyond the end of the file...\n");
		mmapAddr[pageSize] = '2';
	}
	printf("Signals caught:\n"
			"\tSIGBUS : %ld\n"
			"\tSIGSEGV: %ld\n",
			(long) sigBusCaught, (long) sigSegvCaught);
	exit(EXIT_SUCCESS);
}

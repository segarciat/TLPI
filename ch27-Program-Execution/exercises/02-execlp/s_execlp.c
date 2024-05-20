#include "s_execlp.h" /* s_execlp() declaration */
#include <stdarg.h> /* va_list, va_start(), va_arg() va_end() */
#include <sys/types.h> /* size_t */
#include <stddef.h> /* NULL */
#include <stdio.h> /* printf(), snprintf() */
#include <errno.h> /* errno */
#include <stdlib.h> /* malloc(), calloc(), realloc(), free(), getenv() */
#include <string.h> /* strstr() */
#include <unistd.h> /* execve(), confstr() _CS_PATH */
#include <linux/limits.h> /* PATH_MAX, ARG_MAX */

#define SHELL_PATH "/bin/sh"
#define DEFAULT_PATH "/bin:/usr/bin:."

extern char **environ;

static int
execShell(char *argv[], char **envp)
{
	if (argv[0] != NULL) /* Should be intentionally NULL */
		return -1;
	argv[0] = SHELL_PATH; 
	/* First argument is SHELL_PATH, second argument is filename */
	execve(SHELL_PATH, argv, envp);
	return -1;
}

int
s_execlp(const char *filename, const char *arg, ... /*, (char *) NULL */)
{
	va_list varArgp;
	char **argv, **envp;
	/* Should be at least 3: for fallback shell path, for program name, and NULL terminator */
	const size_t ELEMENTS_PER_RESIZE = 10;
	size_t argc = 0;
	size_t argvBufferSize = ELEMENTS_PER_RESIZE;

	if (filename == NULL) {
		errno = EFAULT;
		return -1;
	}
	
	/* Initial guess for number of unnamed arguments received */
	argv = calloc(argvBufferSize, sizeof(char *));
	if (argv == NULL)
		return -1;
	argv[argc++] = NULL;
	argv[argc++] = (char *) filename;

	/* Populate argument vector from unnamed arguments */
	char *p = (char *) arg;
	va_start(varArgp, arg);
	do {
		argv[argc++] = (char *) p;
		if (argc >= argvBufferSize) {
			argvBufferSize += ELEMENTS_PER_RESIZE;
			char **newBuffer = reallocarray(argv, argvBufferSize, sizeof(char *));
			if (newBuffer == NULL) { /* Avoid memory leak */
				free(argv);
				return -1;
			}
			argv = newBuffer;
		}
	} while ((p = va_arg(varArgp, char *)) != NULL);
	argv[argc] = NULL;

	/* Count number of environment variables vector */
	size_t envSize = 0;
	for (char **q = environ; *q != NULL; q++)
		envSize++;

	/* Allocate room for environment and copy environment list */
	envp = calloc(++envSize, sizeof(char *)); /* Extra for NULL terminator */
	if (envp == NULL) {
		free(argv);
		return -1;
	}
	
	/* Assign environment variables and terminate with NULL */
	size_t i = 0;
	for (char **q = environ; *q != NULL && i < envSize - 1; q++, i++)
		envp[i] = *q;
	envp[i] = NULL;
	
	/* See if filename is a path or if we need to search for it */
	if (strchr(filename, '/') != NULL) {
		/* It's a path, exec directly */
		execve(filename, argv + 1, envp); /* argv[0] intentionally NULL */
		if (errno == ENOEXEC)
			execShell(argv, envp);
	} else {
		/* Allocate buffer for PATH string */
		char *pathBuf;
		if ((pathBuf = malloc(ARG_MAX + 1)) == NULL) { /* Extra for additional delimiter character */
			free(argv);
			free(envp);
			return -1;
		}
		
		/* Get PATH from environment, conf, or use fallback */
		size_t pathLen = 0;
		if ((p = getenv("PATH")) != NULL) {
			pathLen = snprintf(pathBuf, ARG_MAX, "%s", p);
		} else if ((pathLen = confstr(_CS_PATH, NULL, (size_t) 0)) != 0) {
			pathLen = confstr(_CS_PATH, pathBuf, ARG_MAX) - 1; /* includes NULL byte in length, so remove it */
		} else {
			pathLen = snprintf(pathBuf, ARG_MAX, "%s", DEFAULT_PATH);
		}
		if (pathLen < ARG_MAX) {
			pathBuf[pathLen++] = ':'; /* overwrite null byte */
			pathBuf[pathLen] = '\0';
		}

		/* Build path to executable */
		char programPath[PATH_MAX];
		char *pathComponent = pathBuf;
		while ((p = strchr(pathComponent, ':')) != NULL) {
			*p = '\0';
			snprintf(programPath, PATH_MAX, "%s/%s", pathComponent, filename);
			execve(programPath, argv + 1, envp);
			if (errno == ENOEXEC) { /* Only try once */
				execShell(argv, envp);
				break;
			}
			pathComponent = p + 1;
		}
		int savedErrno = errno;
		free(pathBuf);
		errno = savedErrno;
	}
	int savedErrno = errno;
	free(argv);
	free(envp);
	errno = savedErrno;
	return -1;
}

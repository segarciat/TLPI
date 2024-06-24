#define _DEFAULT_SOURCE		/* Get crypt() declaration from <unistd.h> */
#define _XOPEN_SOURCE		/* Get getpass() declaration from <unistd.h> */
#include <unistd.h> /* getopt(), optind, opterr, optopt, optarg, getuid(), geteuid(), seteuid(), getpass() */
#include <sys/types.h> /* uid_t */
#include <stdio.h> /* fprintf(), stderr, stdout, printf() */
#include <stddef.h> /* NULL */
#include <stdlib.h> /* exit() EXIT_FAILURE */
#include <pwd.h> /* getpwnam(), struct passwd */
#include <shadow.h> /* struct spwd, getspnam() */
#include <errno.h> /* errno */
#include <string.h> /* strerror(), strcmp() */
#include <sys/resource.h> /* setrlimit(), RLIMIT_CORE, struct rlimit */

extern int optind, opterr, optopt;
extern char *optarg;
extern char **environ;

/* Displays message, the string of the current errno value, and exits */
void
unix_error(char *msg)
{
	fprintf(stderr, "%s: %s\n", msg, strerror(errno));
	exit(EXIT_FAILURE);
}

/* Displays message on how to use this program */
void
displayUsage(FILE *stream, char *programName)
{
	fprintf(stream, "Usage: %s [-h] [-u user] PROGRAM_FILE [ARGS]...\n", programName);
}

int
main(int argc, char *argv[])
{
	int opt, authOk;
	struct rlimit rlim;
	struct passwd *pwd;
	struct spwd *spwd;
	char *user = "root"; /* Default to root */
	char *password, *encrypted;
	uid_t orig_euid, uid;

	/* Drop privilege while not needed */
	uid = getuid();
	orig_euid = geteuid();
	if (uid != 0 && seteuid(uid) == -1)
		unix_error(argv[0]);
	
	/* Call succeeded but credential did not change */
	if (uid != 0 && orig_euid == geteuid()) {
		fprintf(stderr, "%s: Failed to change process credentials\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* Prevent dumping core when process is terminated by signal */
	rlim.rlim_cur = rlim.rlim_max = 0;
	if (setrlimit(RLIMIT_CORE, &rlim) == -1)
		unix_error(argv[0]);

	/* Parse optional -u command-line option */
	while ((opt = getopt(argc, argv, ":hu:")) != -1) {
		switch (opt) {
			case 'u':
				user = optarg;
				break;
			case 'h':
				displayUsage(stdout, argv[0]);
				exit(EXIT_SUCCESS);
			case ':':
				fprintf(stderr, "Missing argument for: -%c\n", optopt);
				exit(EXIT_FAILURE);
			case '?':
				fprintf(stderr, "Unrecognized option: -%c\n", optopt);
				exit(EXIT_FAILURE);
			default:
				fprintf(stderr, "Unexpected case reached while parsing command-line options\n");
				exit(EXIT_FAILURE);
		}
	}
	/* Fail if no program-file was provided */
	if (optind == argc) {
		fprintf(stderr, "%s: Program file was not provided\n", argv[0]);
		displayUsage(stderr, argv[0]);
		exit(EXIT_FAILURE);
	}

	/* Find record for provided user, defaulting to root if -u is not provided */
	pwd = getpwnam(user);
	if (pwd == NULL) {
		fprintf(stderr, "%s: Failed to find record for user: %s\n", argv[0], user == NULL ? "root" : user);
		exit(EXIT_FAILURE);
	}
	
	/* Reclaim privileged credentials and retrieve shadow password record */
	if (seteuid(orig_euid) == -1)
		unix_error(argv[0]);
	spwd = getspnam(user);
	if (spwd == NULL && errno == EACCES) {
		fprintf(stderr, "%s: No permission to read password file\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	if (spwd != NULL)
		pwd->pw_passwd = spwd->sp_pwdp;

	/* Drop privileged credentials while not needed */
	if (seteuid(uid) == -1)
		unix_error(argv[0]);
	if (uid != 0 && orig_euid == geteuid()) {
		fprintf(stderr, "%s: Failed to change process credentials\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* Read user password, encrypt it, and immediately erase cleartext password */
	password = getpass("Password: ");
	encrypted = crypt(password, pwd->pw_passwd);
	for (char *p = password; p != NULL && *p != '\0'; p++)
		*p = '\0';
	if (encrypted == NULL)
		unix_error(argv[0]);

	/* Authenticate against password file */
	authOk = strcmp(encrypted, pwd->pw_passwd) == 0;
	if (!authOk) {
		printf("Incorrect password\n");
		exit(EXIT_FAILURE);
	}

	/* Reclaim privileged credentials to be able to set process credentials to those requested */
	if (seteuid(orig_euid) == -1)
		unix_error(argv[0]);
	if (setgid(pwd->pw_gid) == -1 || setuid(pwd->pw_uid) == -1)
		unix_error(argv[0]);
	if (geteuid() != pwd->pw_uid || getegid() != pwd->pw_gid) {
		fprintf(stderr, "%s: Failed to acquire requested credentials\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* Exec program */
	if (execve(argv[optind], argv + optind, environ) == -1)
		unix_error(argv[0]);
}

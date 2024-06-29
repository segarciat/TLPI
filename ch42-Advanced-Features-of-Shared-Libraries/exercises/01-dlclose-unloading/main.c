#include <stdio.h> /* printf(), fprintf(), stderr */
#include <stddef.h> /* NULL */
#include <dlfcn.h> /* dlopen(), dlerror(), dlerror(), RTLD_NOW */
#include <stdlib.h> /* exit(), EXIT_FAILURE, EXIT_SUCCESS */

int
main(int argc, char *argv[])
{
	void *libmthbasicHandle, *libmthopsHandle;

	libmthbasicHandle = dlopen("libmthbasic.so", RTLD_NOW);
	if (libmthbasicHandle == NULL) {
		fprintf(stderr, "dlopen: %s\n", dlerror());
		exit(EXIT_FAILURE);
	}
	libmthopsHandle = dlopen("libmthops.so", RTLD_NOW);
	if (libmthopsHandle == NULL) {
		fprintf(stderr, "dlopen: %s\n", dlerror());
		exit(EXIT_FAILURE);
	}
	if (argc > 1) {
		printf("Closing libmathops first\n");
		dlclose(libmthopsHandle);
		dlclose(libmthbasicHandle);
	} else {
		printf("Closing libmathbasic first\n");
		dlclose(libmthbasicHandle);
		dlclose(libmthopsHandle);
	}
	exit(EXIT_SUCCESS);
}

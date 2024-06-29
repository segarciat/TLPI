#include <stdio.h>

const double PI = 3.14;

void __attribute__ ((constructor)) mylib_initialize(void)
{
	printf("libmthbasic loaded\n");
}

void __attribute__ ((destructor)) mylib_finalize(void)
{
	printf("libmthbasic unloading\n");
}

/* Returns the sum of x and y */
int add(int x, int y) { return x + y; }

#include <stdio.h>
#include <dlfcn.h>

/* Part of libmthbasic */
extern double PI;

/* Initialize library */
void __attribute__ ((constructor)) initialize(void)
{
	/* Load */
	printf("Loaded libmthops\n");
}

/* Finalize library */
void __attribute__ ((destructor)) finalize(void)
{
	/* Unload */
	printf("Unloading libmathops\n");
}

/* Adds all numbers in the given vector */
int sumv(int numbers[], int len)
{
	/* Part of libmthbasic */
	int sum(int, int);

	int acc = 0;
	for (int i = 0; i < len; i++)
		acc += numbers[i];
	return acc;
}

/* Computes the area of a circle of given radius */
double
area_circle(double radius)
{
	/* PI part of libmthbasic */
	return PI * radius * radius;
}

# Exercise 29.2

Aside from the absence of error checking and various variable and structure declarations,
what is the problem with the following program?

```c
static void *
threadFunc(void *)
{
	struct someStruct *pbuf = (struct someStruct *) arg;
	/* Do some work with structure pointed to by 'pbuf' */
}

int
main(int argc, char *argv[])
{
	struct someStruct buf;

	pthread_create(&thr, NULL, threadFunc, (void *) &buf);
	pthread_exit(NULL);
}
```

## Solution

As discussed in Section 20.4 on page 624, calling `pthread_exit()` from the main thread
allows the remaining threads to continue to execute. However, the main thread provided
a local variable `struct someStruct buf` sa the argument to its sibbling thread.
This variable is stack-allocated, and once it is deallocated upon the main thread
exiting, `threadFunc()` will subsequently point to invalid memory. Moreover, the
main thread exits without detaching itself. Therefore if the new thread does not use
a `pthread_join()` to claim its resources, we will have a resource leak.

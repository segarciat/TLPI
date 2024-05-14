# Exercise 27.5

When we run the following program, we find that it produces no output. Why is this?

```c
#include "tlpi_hdr.h"

int
main(int argc, char *argv[])
{
	printf("Hello world");
	execlp("sleep", "sleep", "0", (char *) NULL);
}
```

## Solution

The `printf()` function is meant to write to the `stdout` stream, which is line-buffered by
default. Since the string passed to it does not contain a newline character, it is not immediately
displayed. When `execlp` is called, it discards the process' stack, data, and heap, so that the
data buffered by the *stdio* library (including the `Hello world` text) is now deleted.
We could address this by flushing the stream before using `execlp()`.

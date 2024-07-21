# Exercise 48-2

Explain why the program in Listing 48-3 incorrectly reports the number of bytes transferred
if the `for` loop is modified as follows:

```c
	for (xfrs = 0, bytes = 0; shmp->cnt != 0; xfrs++, bytes += shmp->cnt) {
		reserveSem(semid, READ_SEM);		/* Wait for our turn */

		if (write(STDOUT_FILENO, shmp->buf, shmp->cnt))
			fatal("write");
		releaseSem(semid, WRITE_SEM);		/* Give writer a turn */
	}
```

## Solution

Even after the `releaseSem()`, the kernel may not preempt the reader until after the
evaluation of the conditional `shmp->cnt != 0`. As a result, 

There is a race condition. If the read from standard input in the writer process has not finished
prior to the reader process starting, the reader will not even go into the loop because `shmp->cnt`
is initialized to 0. Suppose that the kernel preempts  the reader process after the `releaseSem()`
but before `bytes += shmp->cnt` runs. Then the writer process might have enough time to complete
its next `read()` from standard input, changing the `shmp->cnt` value before the reader process
is able to correctly increment `bytes`.

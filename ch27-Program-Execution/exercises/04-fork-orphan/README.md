# Exercise 27.4

What is the effect of the following code? In what circumstances might it be useful?

```c
childPid = fork();
if (childPid == -1)
	errExit("fork1");
if (childPid == 0) { /* Child */
	switch(fork()) {
		case -1: errExit("fork2");

		case  0:			/* Grandchild */
			/* ----- Do real work here ------ */
			exit(EXIT_SUCCESS);				/* After doing real work */

		default:
			exit(EXIT_SUCCESS);				/* Make grandchild orphan */
	}
}

/* Parent falls through to here */

if (waitpid(childPid, &status, 0) == -1)
	errExit("waitpid");

/* Parent carries on to do other things */
```

## Solution

The result is the child is reaped by `waitpid` and the grandchild is adopted by the *init* process
(or in newer systems, the subreaper process such as *systemd*). In turn, its new parent will automatically
reap it when it finishes. It might be useful for performing work that is independent of the parent, which might
involve the same file descriptors. The grandchild may, for example, use `fexecve()` to exec a program
corresponding to a child descriptor given in the parent.

Kerrisk says it may be useful if we are creating a child we cannot wait for to ensure no zombie results.
He further mentions that a parent may exec a program that is not guaranteed to perform a wait.

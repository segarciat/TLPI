# Exercise 24.3

Assuming we can modify the program source code, how could we get a core dump of a process
at a given point in the program, while letting the process continue execution?

## Solution

We can use `fork()` to create a child process at the point of interest. The stack, heap,
and data segment of the child will be a copy of the parent. Then, on the child process,
we can call `abort()`, which sends the child process a `SIGABRT` and causes the core
dump to be generated. The following demonstrates the approach:

```c
pid_t childPid;
switch(childPid = fork()) {
	case -1:
		fprintf(stdderr, "Fork system call failed\n");
		exit(EXIT_FAILURE);
	case 0:
		abort();
	default:
		wait(NULL);
		printf("Continuing execution in parent...");
}
```

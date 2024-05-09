# Exercise 24.1

After a program executes the following series of `fork()` calls, how many new
processes will result (assuming that none of the calls fails)?

```c
fork();
fork();
fork();
```

## Solution

After the first `fork()`, a single a child process is created. The next statement
to be executed by the child process is the second `fork()` call. Though we cannot
guarantee the order in which it happens, we know that both the parent and the child
will execute the second `fork()`, resulting in a two new children. Now four processes
are alive, and all of them will execute the third `fork()` statement, resulting
in four new processes.

At the end, 7 new processes will have been created.

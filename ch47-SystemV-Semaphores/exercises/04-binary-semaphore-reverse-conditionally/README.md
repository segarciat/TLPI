# Exercise 47-4

Add a *reserveSemNB()* function to the code in Listing 47-10 (`binary_sem.c`) to implement the
*reserve conditionally* operation, using the `IPC_NOWAIT` flag.

## Solution

I created a program `main.c` to demonstrate that the implementation is correct. The program
initializes the binary semaphore as in-use (by the parent). I use `fork()` to create a child
that makes a nonblocking attempt to reserve the semaphore, which fails as expected. The following
compiles and runs the program:

```bash
make main
./main
```

The corresponding out looks like:

```
[28873] Parent created semaphore with ID 4
[28873] Parent about to wait for child to exit
[28874] Child about to attempt to reserve semaphore
[28874] Child: As expected, failed to reserve semaphore; already in-use
[28873] Parent reaped child, exiting
[28873] Deleted semaphore with ID 4
```

Verify semaphore is deleted by running `ipcs`.

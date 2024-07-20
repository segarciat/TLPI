# Exercise 47-7

Write a program, analogous to the program in Listing 46-6 (`svmsg_ls.c`, on page 953) that uses
the *semctl()* `SEM_INFO` and `SEM_STAT` operations to obtain and display a list of all semaphore
sets on the system.

## Solution

Compile and run:

```bash
gcc svsem_ls.c
./a.out
# Compare with ipcs output
ipcs -s
```



# Exercise 50-1

Write a program to verify the operation of the `RLIMIT_MEMLOCK` resource limit by writing
a program that sets a value for this limit and then attempts to lock more memory than the
limit.

## Solution

According to Section 36.3 and 50.2, the `RLIMIT_MEMLOCK` limit defines a limit on the number
of bytes that a process can lock into memory. Starting Linux 2.6.9, an unprivileged process
can lock small amounts of memory up to the soft limit indicated by `RLIMIT_MEMLOCK`. For a
privileged process, the limit is ignored.

As described in Section 50.2, the `RLIMIT_MEMLOCK` limit applies in units of pages, and
when performing limit checks, `RLIMIT_MEMLOCK` is rounded *down* to the nearest multiple
of the system page size.

To compile and run my solution:

```bash
gcc -Wall memlock_limit.c
./a.out
```

Sample output:

```
System page size: 4096
RLIMIT_MEMLOCK (current): 2075578368
Attempting to lock 8192 bytes of virtual memory
Successfully locked requested memory region. Unlocking
Lowering the hard limit of RLIMIT_MEMLOCK to one system page
Attempting to lock two system pages of virtual memory
mlock() - Failed to lock requested memory region: Cannot allocate memory
This error was expected
```

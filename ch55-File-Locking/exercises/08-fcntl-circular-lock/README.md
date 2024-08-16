# Exercise 55-8

Use the program in Listing 55-2 (`i_fcntl_locking.c`) to demonstrate
that the kernel detects circular deadlocks involving three (or more)
processes locking the same file.

## Solution

I began by compiling the program:

```bash
make i_fcntl_locking
```

Then I made a test file:

```bash
fallocate -l 64K test_lock_file
```

Finally I ran three processes locked three byte ranges:

- Process 1 [PID=27859] gets a write lock for the bytes at positions 10 through 12: `w w 10 3`
- Process 2 [PID=27999] gets a write lock for the bytes at positions 13 through 15: `w w 13 3`
- Process 3 [PID=28013] gets a write lock for the bytes at positions 16 through 18: `w w 16 3`

At this point all have acquired a lock. Then, each process makes the following
blocking attempts:

- Process 1 requests a write lock on bytes 13 through 15 with `w w 13 3` and blocks because process 2 has a lock.
- Process 2 requests a write lock on bytes 16 through 18 with `w w 16 3` blocks because process 3 has a lock.
- Process 3 requests a write lock on bytes 10 through 12 with `w w 10 3` and fails immediately.

The message observed by process 3 is:

```
[PID=28013] failed (deadlock)
```

I issued the following command afterwards to see why the lock cannot be obtained:

```
>>> PID=28013 > g w 0 0
```

I observed:

```
[PID=28013] Denied by WRITE lock on 10:3 (held by PID 27859)
```

as expected. Control returns to process with PID 28013, but the other two processes remain
blocked.

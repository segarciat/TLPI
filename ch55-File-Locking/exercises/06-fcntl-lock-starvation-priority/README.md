# Exercise 55-6

Experiment with the program in Listing 55-2 (`i_fcntl_locking.c`) to verify the
statements made in Section 55.3.6 regarding lock starvation and priority for
*fcntl()* record locks.

## Solution

To begin experimenting, I compiled `i_fcntl_locking.c`:

```bash
make i_fcntl_locking
```

Then I created a dummy test file:

```bash
fallocate -l 64K test_lock_file
```

In all of lock interactions, I specify `0 0` to mean "start at byte 0, and
go until the end of the file". In other words, a lock on the entire file is requested.

### Lock starvation

I verified that a process waiting for a write lock may be starved by a series of processes
placing read locks on the same file.

I began by creating a process that running one instance of `i_fcntl_locking` that
placed a read lock on the file by passing `s r 0 0`:

```
File       Locking
----       -------
test_lock_file advisory

Enter ? for help
PID=25708> ?

Format: cmd lock start length [whence]

    'cmd' is 'g' (GETLK), 's' (SETLK), or 'w' (SETLKW)
        or for OFD locks: 'G' (OFD_GETLK), 'S' (OFD_SETLK), or 'W' (OFD_SETLKW)
    'lock' is 'r' (READ), 'w' (WRITE), or 'u' (UNLOCK)
    'start' and 'length' specify byte range to lock
    'whence' is 's' (SEEK_SET, default), 'c' (SEEK_CUR), or 'e' (SEEK_END)

PID=25708> s r 0 0
[PID=25708] got lock
PID=25708>
```

Then I ran another instance of the program that makes a blocking request to acquire a write
lock by leveraging the `SETLWK` command. To obtain this behavior, I issued the command `w w 0 0`:

```
PID=25799> w w 0 0

```

I ran a third instance of `i_fcntl_locking` that specifies `s r 0 0` to get a read lock on the
file, which succeeded immediately:

```
PID=25854> s r 0 0
[PID=25854] got lock
PID=25854> 
```

At this point, I released the first read lock:

```
[PID=25708] got lock
PID=25708> s u 0 0
[PID=25708] unlocked
PID=25708> 
```

Meanwhile, the process that requested the write lock, `PID=25799`, is still blocked. In this
way, I could create another process requests a read lock, and then release the read lock on
`PID=25854`. Or, I could reacquire the read lock on the first process with `PID=25708`.
In this way, `PID=25799` is starved of the write lock.

### Priority

I tried testing this by doing the following:

- Execute a process that acquires a write lock with: `w w 0 0`.
- Execute a second process that makes a blocking attempt to acquire a write lock with: `w w 0 0`.
- Execute a third process that makes a blocking attempt to acquire a read lock with: `w r 0 0`.
- On the first process, release the write lock with: `s u 0 0`.

Most of the times, I observed that the process waiting foe the write lock was granted the lock.
However, at times, the process waiting on the read lock got the lock. From this I concluded
that neither readers nor writers have priority over one another for a lock.

# Exercise 55-5

In Section 55.3.4, we noted that, on Linux, the time required to add or check for
the existence of a lock is a function of the position of the lock in the list of
all locks on the file. Write two programs to verify this.

a) The first program should acquire (say) 40,001 write locks on a file. These locks
are placed on alternate bytes of the file; that is, locks are placed on bytes 0, 2,
4, 6, and so on through to (say) byte 80,000. Having acquired these locks, the process
then goes to sleep.

b) While the first program is sleeping, the second program loops (say) 10,000 times,
using `F_SETLK` to try to lock one of the bytes locked by the previous program (these
lock attempts always fail). In any particular execution, the pogram always tries
to lock bye `N * 2` of the file.

Using the shell built-in *time* command, measure the time required by the second program
for *N* equals 0, 10,000, 20,000, 30,000, and 40,000. Do the results match the expected
linear behavior?

## Solution

Begin by compiling the program for part a) by running

```bash
gcc -Wall -o lock_sleep lock_sleep.c
```

The program expects a single command-line argument, which is a file with at least 80,001
bytes. The program does what is described in part a), which is to lock the even bytes
from positions 0 through 40,001. Keep in mind that *fnctl()* allows locking nonexistent
bytes past the end of the file, so even if the file is not large enough, the program will
not complain.

Then compile the program for part b) by running:

```bash
gcc -Wall -o acquire_write_lock acquire_write_lock.c
```

The program expects two command-line arguments, which is a file to lock, and a positive
integer *N*. The integer represents the `2*N`-th byte in the file which is to be locked.
The program attempts, and fails, to lock that byte 10,000 times.

To test the program, I began by creating a file with at least 80,000 bytes with
the `fallocate` program:

```bash
fallocate -l 100K test_lock_file
```

Then I ran `./lock_sleep` in the background:

```bash
./lock_sleep test_lock_file &
```

I waited for confirmation that all bytes were locked:

```
[43896] Locking even bytes in file through position 80000
[43896] Successfully locked bytes even byte positions through 80000
[43896] Pausing execution...
```

The program `acquire_write_lock` can be run as follows:

```bash
./acquire_write_lock test_lock_file 1500
```

Here is sample output:

```
[43947] Proceeding to attempt to lock file 10000 times
[43947] Number of successful write lock acquisitions: 0 out of 10000
```

Then I used a shell script `lock_fail_timings.sh` that timed `acquire_write_lock` using
the `time` command and displayed the system (kernel) time spent by the process attempting
to lock the specified byte 10000 times.

```bash
chmodu +x lock_fail_timings.sh
./lock_fail_timings.sh
```

The output was:

```
       N  SYS_TIME
--------------------------------
       0  0m0.024s
    5000  0m0.345s
   10000  0m0.660s
   15000  0m0.952s
   20000  0m1.272s
   25000  0m1.623s
   30000  0m2.334s
   35000  0m3.574s
   40000  0m4.357s
```

The output shows that it does indeed take longer to lock later bytes. The result
is not exactly linear. Once the experiment was done, I terminate the paused process:

```bash
kill 43896
```

# Exercise 55-1

Experiment by running multiple instances of the program in Listing 55-1
(`t_flock.c`) to determine the following points about the operation of *flock()*:

a) Can a series of processes acquiring shared locks on a file starve a process attempting
to place an exclusive lock on the file?

b) Suppose that the file is locked exclusively, and other processes are waiting to place
both shared and exclusive locks on the file. When the first lock is released, are there
any rules determining which process is next granted a lock? For example, do shared locks
have priority over exclusive locks or vice versa? Are locks granted in FIFO order?

c) If you have access to some other UNIX implementation that provides *flock()*, try to
determine the rules on that implementation.

## Solution

I began by compiling `t_flock.c`:

```bash
make t_flock
```

Then I wrote simple shell scripts to conduct the experiments.

a) Yes, processes acquiring shared locks on a file locks on a file can indeed starve a process
attempting to place an exlcusive lock. To investigate this, I designed a simple script,
`exp_a.sh`, that began by spawning two process running the `t_flock` program to acquire a
file lock on this README.md file. The first process requests and immediately acquires a shared
lock for 2 seconds, while the  second requests an exclusive lock for 2 seconds. Then,
I entered a `for` loop that, at 1 second intervals, spawns a new process that requests a shared
lock on the same file for 2 seconds also. This way, halfway through the lease time on the lock
of the previously spawned process, a new process is spawned that immediatley acquires the lock.
The result is that at all instant, at least one process holds a shared lock.

The following writes the sequence of grants to a file and displays the last entry, indicating
the process that acquires the lock last

```bash
chmod u+x exp_a.sh
./exp_a.sh | grep granted > exp_a.out
tail -n 1 ./exp_a.out
```

I consistently observed that the output was the single entry with `LOCK_EX`:

```
PID 15017: granted    LOCK_EX at 20:31:58
```

b) I observed FIFO behavior. I wrote two simple scripts in which I began by spawning a
process that runs `t_flock` to request and acquire an exclusive lock. Then I entered a
`for` loop that spawned processes waiting to acquire locks. 

i) The script `exp_b_alt.sh` spawns processes in an alternate pattern. That is, each loop
spawns 2 processe, the first requesting an exclusive lock and the second requesting a
shared lock. By the end, 20 process are waiting to acquire an exclusive lock, and 10
are waiting to acquire a shared lock, but their scheduling is alternated. In this case,
the first process requesting an exclusive lock obtains the lock, but once the first process
requesting a shared lock obtains its lock, all of the other process obtain it immediately
as well, and the processes requesting an exclusive lock must wait. This is similar to the
situation in experiment a).

ii) The second script `exp_b_ex_first.sh` spawns 5 processes requesting an exclusive lock,
and then it spawns 5 processes requesting a shared lock. I observed that the processes
waiting to obtain an exclusive lock all acquire and release their locks before a process
waiting to acquire a shared lock is granted the lock.

```sh
chmod u+x exp_b_alt.sh
./exp_b_alt.sh | grep granted > exp_b_alt.out
cat exp_b_alt.out

chmod u+x exp_b_ex_first.sh
./exp_b_ex_first.sh | grep granted > exp_b_ex_first.out
cat exp_b_ex_first.out
```

c) Skipping.

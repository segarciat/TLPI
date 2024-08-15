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
attempting to place an exclusive lock on the same file. To investigate this, I designed a simple
script, `exp_a.sh`, that begins by spawning two process running the `t_flock` program to acquire a
lock on this `README.md` file. The first process requests and immediately acquires a shared lock for
2 seconds, while the  second requests an exclusive lock for 2 seconds and blocks. Then, the script
enters a `for` loop in which, at 1 second intervals, it spawns a new process that requests a shared
lock on the same file for 2 seconds also. The timing is like this:

**0 Seconds**: A shared lock is granted immediately to process 1 for 2 seconds, and process 2 blocks waiting
on exclusive lock.
**1 Second **: Process 3 requests and acquires a shared lock for 2 seconds.
**2 Seconds**: Process 1 releases shared lock, and process 4 requests and is granted a shared lock for 2 seconds.
**3 seconds**: Process 3 releases shared lock, and process 5 requests and is granted a shared lock for 2 seconds.

and so on. This way, a shared lock is held long enough that a new process is able to acquire it
for 2 seconds, and in effect, each new process effectively lengthens the duration that a shared
lock is held on the file by 1 second.

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

b) I observed FIFO behavior. I wrote two simple scripts, which begin by spawning a process that 
runs `t_flock` to request and acquire an exclusive lock. Then each script enters a `for` loop that
spawns processes that, depending on the lock request, may block.

i) The script `exp_b_alt.sh` spawns processes in an alternate pattern. That is, each loop
spawns 2 processes, the first requesting an exclusive lock and the second requesting a
shared lock. By the end, 20 process are waiting to acquire an exclusive lock, and 10
are waiting to acquire a shared lock. In this case, the first process requesting an exclusive
lock obtains the lock. After the exclusive lock is released, the next process in queue is one waiting
for a shared lock. At that point, all of the process that requested a shared lock are granted a lock,
while the process that requested an exclusive lock remain blocked, similar to a). I suspected
from this experiment that the lock granting policy adhered to a FIFO pattern.

ii) The second script `exp_b_ex_first.sh` spawns 5 processes requesting an exclusive lock,
and then it spawns 5 processes requesting a shared lock. I observed that the processes
waiting to obtain an exclusive lock all acquire and release their locks before a process
waiting to acquire a shared lock is granted the lock. If the shared locks had a priority, then
one of the 5 processes waiting for a shared lock would have obtained the lock first. Since
this did not occur, this further suggests FIFO lock granting behavior.

```sh
chmod u+x exp_b_alt.sh
./exp_b_alt.sh | grep granted > exp_b_alt.out
cat exp_b_alt.out

chmod u+x exp_b_ex_first.sh
./exp_b_ex_first.sh | grep granted > exp_b_ex_first.out
cat exp_b_ex_first.out
```

c) Skipping.

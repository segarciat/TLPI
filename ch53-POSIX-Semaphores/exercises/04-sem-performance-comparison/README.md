# Exercise 53-4

In Section 53.5, we noted that POSIX semaphores perform much better than System V semaphores
in the case where the semaphore is uncontended. Write two programs (one for each semaphore type)
to verify this. Each program should simply increment and decrement a semaphore a specified
number of times.

## Solution

### System V Semaphore

I used `svsem_create.c` from the source distribution of the book to create a System V semaphore set.
To compile:

```bash
make svsem_create
```

To create the System V semaphore set with a single semaphore in the set, I specified `-p` and
`1`, where the `-p` is to request that the program use a private key, guaranteeing that the key
used to create the IPC object is unique. The `-cx` is so that the semaphore is exclusively created
by this process:

```bash
./svsem_create -pcx 1
```

The output in my case was `0`, indicating that the ID System V IPC ID for the semaphore set is `0`.
To compile `svsem_incr.c`, I ran:

```bash
gcc -Wall -std=gnu11 -o svsem_incr svsem.c util.c
```

Finally, I ran the program so that it would increment and decrement the semaphore value 10 million
times, and timed the process execution time with the `time` command:

```bash
time ./svsem_incr 0 10000000
```

The output was

```
[PID 31594] Incremented and decremented value of semaphore a total of 10000000 times

real	0m3.550s
user	0m0.755s
sys	0m2.794s
```

### POSIX Semaphore

I used `psem_create.c` from the source distribution of the book to create a POSIX semaphore. To compile:

```bash
make svsem_create
```

I created a POSIX semaphore named `/mysem` by specifying `-cx`:

```bash
./psem_create -cx /mysem
```

To compile `svsem_incr.c`, I ran:

```bash
gcc -Wall -std=gnu11 -o psem_incr psem_incr.c util.c
```

Finally, I ran the program so that it would increment and decrement the semaphore value 10 million
times, and timed the process execution time with the `time` command:

```bash
time ./psem_incr /mysem 10000000
```

The output was

```
[PID 31620]: Posted to and wait for semaphore a total of 10000000 times

real	0m0.194s
user	0m0.194s
sys	0m0.001s
```

### Cleaning Up

To remove the System V semaphore, run `ipcrm -s` and provide the ID of the semaphore created:

```bash
ipcrm -s 0
```

To delete the POSIX semaphore, delete it from the *tmpfs* file system mounted at `/dev/shm`:

```bash
rm /dev/shm/sem.mysem
```


### Conclusion

This simple test demonstrates that when there is no contention, POSIX semaphores
outperform System V semaphores.

# Exercise 53-2

Modify the program in Listing 53-3 (`psem_wait.c`) to use *sem_timedwait()* instead of
*sem_wait()*. The program should take an additional command-line argument that speciies
a (relative) number of seconds to be used as the timeout for the *sem_timedwait()* call.

## Solution

An application using POSIX semaphores must be compiled with `cc -pthread`, and since POSIX
clocks are used, we also need to link against *librt* using the `-lrt` option.

```bash
make psem_create
make psem_wait

./psem_create -cx /psem_52_3
ls /dev/shm/
```

Notice that in Linux the *tempfs* filesystem houses the semaphores created. Each name is prefixed
by `sem.`, so the semaphore just created has name `sem.psem_52_3`. Finally:

```bash
# Block wait for 3 seconds
./psem_wait /psem_52_3 3
```

# Exercise 53-3

Devise an implementation of POSIX semaphores using System V semaphores.

## Solution

The key functions exposed by the POSIX semaphore implementation are the following:

```c
#include <fcntl.h>      /* Defines O_* constants use for oflag parameter */
#include <sys/stat.h>   /* Defines mode constants used for permissions */
#include <semaphore.h>

/* Open an existing semaphore, or create a new semaphore */
sem_t *sem_open(const char *name, int oflag,
					..., /* mode_t mode, unsigned int value */);

/* Close a currently opened semaphore */
int sem_close(sem_t *sem);

/* Remove an semaphore */
int sem_unlink(const char *name);

/* Decrement semaphore value by 1. If value is greater than 0, return immediately.
   Otherwise, block until its value is greater than 0 */
int sem_wait(sem_t *sem);

/* Like sem_wait(), but if the attempt would block, it fails immediately with errno EAGAIN */
int sem_trywait(sem_t *sem);

/* Like sem_wait(), but blocks until the specified time, and if the timeout expires, fails with ETIMEDOUT */
int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout);

/* Increment semaphore value by 1 and return */
int sem_post(sem_t *sem);

/* Get the current value of the semaphore and return its value in the sval buffer */
int sem_getvalue(sem_t *sem, int *sval);
```

I implemented a similar API, except that I used the name pattern `psem_*` instaed of `sem_*`
so as to not clash with the actual glibc API. The following are some noteworthy aspects of
my implementation:

1. The pathname given to `psem_open()` used to create a file under `/tmp`, which in turn is
used by `ftok()` to create an IPC key to be able to associate with an IPC ID for the System
V IPC semaphore object. The semaphore itself has kernel persistence, so it will be deleted
on System restart. However, the file will remain in `/tmp` until it is deleted, so it may
outlive the semaphore. On the other hand, if the file is deleted while the IPC object still
exists, this may cause problems when using the filename as the key to get the semaphore.

2. My implementation of `psem_unlink()` immediately deletes the semaphore. This means it does
not fulfill the requirement that it should be only marked for deletion, and that actual deletion should
occur when all processes have ceased using the semaphore. I did it this way for simplicity.
On success, it also deletes the temporary file created during `psem_open()`.

There are other bugs and inefficiences. For example, the temporary file does not need to remain
open while using the semaphore, and keeping it open only serves to reduce the number of available
file descriptors (in essence it's a resource leak).

Compile and run the program as follows.

```bash
gcc -Wall main.c psem.c -lrt
./a.out
```

The outut is as follows:

```
Successfully opened semaphore initiaized (should be 1)
Successfully retrieved semaphore value: 1 (as expected)
Successfully did a semaphore post.
Successfully retrieved semaphore value: 2 (as expected)
Timed wait returned immediately
Successfully closed semaphore.
Successfully removed semaphore.
```

If an error occurs, you can use `ipcs` to identify the semaphore created, and `ipcrm` to remove it.
You can also remove the temporary file created in `/tmp` by the program.

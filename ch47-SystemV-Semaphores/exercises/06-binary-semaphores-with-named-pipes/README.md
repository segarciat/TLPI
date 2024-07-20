# Exercise 47-6

Implement a binary semaphores protocol using named pipes. Provide functions to reserve, release,
and conditionally reserve the semaphore.

## Solution

To implement the semaphore, I used a FIFO (a named pipe), as specified. To reserve a FIFO, I used
the fact that reading from a FIFO blocks when it has 0 bytes.

I provided a function *initSemAvailable()* to initialize usage of the FIFO as a semaphore. The
function opens the FIFO for both read (using `O_NONBLOCK`) and write, and then writes a byte
to the pipe to ensure that the first attempt to reserve it with *reserveSem()* or *reserveSemNB()*
is successful. From here I got stuck.

My initial attempt was to open the named pipe twice, once for read and once for write.
Then I would write 1 byte to the pipe, and subsequently close both file descriptors. The *reserveSem()*
operation would then block because there is no FIFO write file descriptor open. I then attempted
to use `O_NONBLOCK` to allow opening anyway (for reading), and then used the `F_SETFL` operation of `fcntl()` to
remove the `O_NONBLOCK` flag after opening. However, the *read()* call would then block anyway, presumably
because the byte written during initialization of the pipe as a semaphore was discarded from the kernel
buffer once all file descriptors had been closed.

In my second attempt, I decided that these pipe descriptors should be kept open after *initSemAvailable()*.
For this, I added a second argument to the function, a pointer to structure of type `struct sempipe`, which saved
the two descriptors that were kept open. This had the following effects:

1) In *reserveSem()*, I no longer needed to provide `O_NONBLOCK` when opening the FIFO for reading,
because an extra file descriptor was held open for writing as a result of my modified *initSemAvailable()*.

2) The *read()* within *reserveSem()* no longer blocked because the byte written during initialization
was not flushed (since the descriptors remained open after the write during *initSemAvailable()*).

3) In *releaseSem()*, the attempt to open the semaphore for writing succeeds immediately because of
the extra file descriptor kept open for reading. also, the *write()* is not subject to `SIGPIPE`
due to writing to a FIFO whose associated read end is not open (or `EPIPE` if that signal is
handled or ignored).

Attempting to release the semaphore more than once in succession (that is, before another process
or thread has had the opportunity to reserve it) may result in undefined behavior. For example,
if the semaphore is released twice, then the FIFO will have 2 bytes. The first reservation attempt
succeed, and so will the second, even if the caller that first reserved it has not yet released it.

Try the sample program:

```bash
gcc main.c fifo_binary_sem.c
./a.out
```

Sample output:

```
./a.out
FIFO created: ./binary_semaphore.fifo
[88553] Semaphore was immediately available! Sleeping for a second...
[88554] Semaphore already in-use, waiting to use...
[88553] Done sleeping.
[88553] Ok, releasing semaphore
[88553] Waiting on child...
[88554] Acquired semaphore on second attempt!
[88554] Ok, releasing semaphore
[88553] Reaped child. Parent disabling semaphore.
[88553] Successfully removed FIFO: ./binary_semaphore.fifo
```

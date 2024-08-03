# Exercise 52-1

Rewrite the programs in Listing 48-2 and Listing 48-3 (Section 48.4) as a threaded
application, with the two threads passing data to each other via a global buffer,
and using POSIX semaphores for synchronization.

## Solution

Listing 48-2 and 48-3 employed a System V shared memory under the control of a System
V semaphore to transfer data between a reader and a writer process. The writer process
was programmed to copy data from standard input into the shared memory segment. The reader
process was programmed to read data from the shared memory segment and write it to standard
output.

To achieve this in a threaded application, I employed a main thread that allocates a buffer
and initializes an unnamed semaphore that controls access to the buffer. The main thread
then acquires the semaphore and spawns a thread. The peer thread blocks while waiting to
acquire the semaphore, and the main thread reads from standard input in the mean time.
Once it has read sufficient data, the main thread releases the semaphore and the peer
thread writes the data.

```bash
make
./psem_xfr < README.md
```

This prints the `README.md` file and the number of bytes transferred.

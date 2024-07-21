# Exercise 48-1

Replace the use of binary semaphores in Listing 48-2 (`svshm_xfer_writer.c`) and Listing 48-3
(`svshm_xfr_reader.c`) with the use of event flags (Exercise 47-5)

## Solution

I used my solution from Exercise 47-5. Moreover, it takes more work to use the event flags.
Normally, reserving a semaphore decreases the semaphore value at the samea time. However,
waiting for the event flag to clear does not change the underlying semaphore value.
Thus, the event flag must be cleared after having waited for it to be set.

We can compile and test the program as follows:

```bash
make shm_xfr_writer
make shm_xfr_reader

./shm_xfr_writer < Makefile &
./shm_xfer_reader
```

The result is that the content of `Makefile` is printed to standard output.

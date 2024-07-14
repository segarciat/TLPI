# Exercise 44.7

Write programs to verify the operation of nonblocking opens and nonblocking I/O on FIFOs
(see Section 44.9).

## Solution

I wrote a program to demonstrate each of the following:

- When a process opens that opens a FIFO for reading species the `O_NONBLOCK` flag,
it succeeds in opening the FIFO, regardless of whether another process has already
opened the FIFO for writing.

- Reading from a FIFO that has been blocked with `O_NONBLOCK` for reading while no
writer has written to the FIFO or opened it for writing will return EOF.

- When a process that opens a FIFO for writing specifies the `O_NONBLOCK` flag,
it fails with `errno` set to `ENXIO` if no other process holds an open file descriptor
for the FIFO for reading.

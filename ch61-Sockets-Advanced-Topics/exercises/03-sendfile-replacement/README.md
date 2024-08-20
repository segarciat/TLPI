# Exercise 61-3

Implement a replacement for *sendfile()* using *read()*, *write()*, and *lseek()*.

## Solution

As discussed in Section 61.4, *sendfile()* is designed to eliminate the inefficiency associated
with transferring the contents of a file to a socket. Specifically, instead of having to transfer
the contents from disk, to the kernel buffer cache, to a user-space buffer, back to a kernel buffer
(the socket send buffer) and to the network, the transfer to user space is eliminated.

Roughly, the system call expects the input file descriptor of a file that can be memory-mamaped
(i.e. a regular file) and the output file descriptor to refer to a socket. The operation is affected
by whether the *offset* pointer it is provided is `NULL` or not. See *man 2 sendfile* for details.

My implementation of course does not maintain the data in the kernel space since *read()* and *write()*
transfer data between user and kernel space. Moreover, I employed the *readn()* and *writen()* from
the `rdwrn.h` functions in Listing 61-1 provided with the code distribution of the book.

I included a server and client program to verify my implementation.

- The server program creates a TCP socket listening on port `50000`. After accepting a connection,
it displays the address of the client and displays all contents it receives from the client to
standard output.

- The client expects a host and filename as arguments. It opens the file, connects to the host
at port `50000`, and uses `sendfile_rwl()` to send the entire file to the host.

Compile the client and server as follows:

```bash
make isock_cat_sv
make isock_cat_cl
```

Then run the server in the background and run the client:

```bash
./isock_cat_sv &
./isock_cat_cl localhost README.md
```

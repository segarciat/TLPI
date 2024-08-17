# Exercise 57-1

In Section 57.3, we noted that UNIX domain datagram sockets are reliable.
Write a program to show that if a sender transmits datagrams to a UNIX
domain datagram socket faster than the receiver reads them, then the sender
is eventually blocked, and remains blocked until the receiver reads some of
the pending datagrams.

## Solution

### Setup

I wrote a server and a client program to demonstrate this. The server
is the reader, and for simplicity, the address associated with its
UNIX domain socket is in the `/tmp` directory, although note that this
is generally unsafe in production.

After the server binds its socket address to the filesystem path, it
enters a loop where it does the following:

1. It blocks waiting for input via the *recvfrom()* system call.
2. It sleeps for about 1 second, simulating some compute operation.
3. It displays data to standard output.

The server loops forever, and need to be terminated by using a software
signal, such as a `SIGINT`, perhaps with the `kill` command or a terminal-generated
signal.

Conversely, the client creates it socket, builds the server's socket
address structure, and loops doing the following:

1. Reads data from standard input.
2. Uses the *sendto()* system call to send the data to the server's socket.
3. Displays the stdout how many bytes were sent, and when.

The client continues until it has exhausted all data from stdin.

### Running the Demo

Begin by compiling the client and server programs:

```bash
make sun_dgram_sv
make sun_dgram_cl
```

Both programs are linked agaisnt *librt* because they use the POSIX clocks.
They are also linked against `errorexit.c`, which provides a simple version to
display a message and exit.

I began by running the server program in the background:

```bash
./sun_dgram_sv > result_file &
```

The server program redirects all input that it receives to a file called
`result_file`. Then, I created a 100KiB file:

```bash
fallocate -l 100K large_file
```

Finally I ran the client in that background such that its standard input
comes from `large_file`:

```bash
./sun_dgram_cl < large_file
```

The client sends all of the data to the server and displays a message on
stdout saying when it sent the data, and how much data it sent. I observed that
the client blocked for a while before it was able to send data again, and observed
the following output:

```
... # More timestamps up here
[10277]: Sent 1024 bytes at: Sat Aug 17 09:28:29 2024
[10277]: Sent 1024 bytes at: Sat Aug 17 09:28:29 2024
[10277]: Sent 1024 bytes at: Sat Aug 17 09:29:39 2024
[10277]: Sent 1024 bytes at: Sat Aug 17 09:29:39 2024
[10277]: Sent 1024 bytes at: Sat Aug 17 09:29:39 2024
[10277]: Sent 1024 bytes at: Sat Aug 17 09:29:39 2024
[10277]: Sent 1024 bytes at: Sat Aug 17 09:29:39 2024
[10277]: Sent 1024 bytes at: Sat Aug 17 09:29:39 2024
```

From the last few lines of output, it's evident the client had to wait
about 10 seconds before continuing to send data. Eventually the client,
so I proceeded to verify whether the data was sent accurately:

```bash
diff result_file large_file
echo $?
```

The exit status was 0, reflecting the fact that the files have the same
content.

# Exercise 63-2

Write an *echo* server (see Section 60.2 and 60.3) that handles both TCP and UDP
clients. To do this, the server must create both a listening TCP socket and a UDP
socket, and then  monitor both sockets using one of the techniques described in
this chapter.

## Solution

I wrote `echo_sv.c`, which begins by creating a TCP socket and a UDP socket using
the internet domain library developed in Section 59.12. Then I proceed to using
`fcntl()` to set the `O_NONBLOCK` flag on the socket file descriptors.
In preparation to using the *poll()* system call for I/O  multiplexing, I add
both to a `struct pollfd` list, and set the interest list of events to `POLLIN`.

When *poll()* informs us that the listening file descriptor has input available,
this signifies a new connection. The server proceeds to forking a child to handle
this request concurrently.

On the other hand, when *poll()* informs us that the UDP socket file descriptor
has input available, this signifies that a socket datagram is available.
These requests are handle interatively (a child is not forked to handle them).

To simplify testing, I used `is_echo_cl.c` from Listing 61-2 as a TCP echo client
(which I renamed to `tcp_echo_cl.c`) and `id_echo_cl.c` from Listing 60-3 as a
UDP echo client (which I renamed to `udp_echo_cl.c`).

Compile all three programs:

```bash
make echo_sv
make tcp_echo_cl
make udp_echo_cl
```

On one terminal window, run the server in the foreground:

```bash
sudo ./echo_sv
```

Notice that since the *echo* service corresponds to port 7, and since this is a privileged
port, it is necessary to run the server with privilege.

To run the TCP client, provide standard input:

```bash
./tcp_echo_cl localhost < README.md
```

To run the UDP client, provide messages as separate command-line arguments:

```bash
./udp_echo_cl localhost hello world im sergio
```

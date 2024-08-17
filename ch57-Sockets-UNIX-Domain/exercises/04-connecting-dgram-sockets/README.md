# Exercise 57-4

Suppose that we create two UNIX domain datagram sockets bound to the paths
`/somepath/a` and `/somepath/b`, and that we connect the socket `/somepath/a`
to `/somepath/b`. What happens if we create a third datagram socket and try
to send (*sendto()*) a datagram via that socket to `/somepath/a`? Write a program
to determine the answer. If you have access to other UNIX systems, test the
program on those systems to see if the answer differs.

## Solution

Section 56.6.2 explains that after a datagram socket has been connected:

- Datagrams can be sent through the socket using *write()* (or *send()*)
and are automatically sent to the peer process.

- Only datagrams sent by the peer may be read on the socket.

Kerrisk also points out that the effect of *connect()* is asymmetric, applying
only to socket that called *connect()*, and not necessarily to the peer,
unless the peer has also done the same. In this exercise, both sockets call
*connect()* after binding their sockets to a filesystem path.

I wrote a server program that creates the two sockets binds them at their different
(respective) paths, and connects them to each other using *connect()*. Not knowing
what happens ahead of time, I was prepared to read data at both sockets, so the
main loop first waits to receive a message on socket A, and then on socket B.
The server displays all messages received on stdout, and does not respond to
the client.

The client, on the other hand, accepts command-line arguments, and writes them
all to socket A. Once it has written all messages, it exits.

To compile the server, and client, run:

```bash
make so_dgram_conn_sv
make so_dgram_conn_cl
```

Run the server on the foreground:

```bash
./so_dgram_conn_sv
```

which just suspends (blocks) and outputs:

```
Waiting for message on socket A
```

On a different terminal session, I ran the client as follows:

```bash
./so_dgram_conn_cl hello world
```

I observed that the client's attempt to use *sendto()* failed with `EPERM`:

```
Failed to connect to server socket: Operation not permitted
```

# Exercise 61-5

Write a client and server that permit the client to execute arbitrary shell commands on
the server host. (If you don't implement any security mechanism in this application, you
should ensure that the server is operating under a user account where it  can do no damage
if invoked by malicious users.) The client should be executed with two command-line arguments:

```
$ ./is_shell_cl server_host 'some-shell-command'
```

After connecting to the server, the client sends the given command to the server, and then closes
its writing half of the socket using *shutdown()*, so that the server sees end-of-file. The server
should handle each incoming connection in a separate child process (i.e., a concurrent design).
For each incoming connection, the server should read the command from the socket (until end-of-file),
and then exec a shell to perform the command. Here are a couple hints:

- See the implementation of *system()* in Section 27.7 for an example of how to execute a shell command.
- By using *dup2()* to duplicate the socket on standard output and standard error, the execed command
will automatically write to the socket.

## Solution

To simplify the coding, I used `inet_sockets.h` and `inet_sockets.c` from Listing 59-8 and 59-9
which come with the distribution of the book. I also used the `rdwrn.h` from Listing 61-1, and
`tlpi.h`. I used a TCP socket for the server and client, bound to port `50000` by default, defined in
`is_shell.h`.

I did not implement a security mechanism for simplicity, so the server should not be run outside
of playing with this exercise.

To compile both programs:

```bash
make is_shell_sv
make is_shell_cl
```

On one shell session, start the server in the foreground, which blocks waiting on connections:

```bash
./is_shell_sv
```

On a different shell session (ideally in a different terminal), make a request to the server
using the client:

```bash
./is_shell_cl localhost 'tail /etc/services'
```

The server responds with all of output, which the client sees:

```
sgi-cad		17004/tcp			# Cluster Admin daemon
binkp		24554/tcp			# binkp fidonet protocol
asp		27374/tcp			# Address Search Protocol
asp		27374/udp
csync2		30865/tcp			# cluster synchronization tool
dircproxy	57000/tcp			# Detachable IRC Proxy
tfido		60177/tcp			# fidonet EMSI over telnet
fido		60179/tcp			# fidonet EMSI over TCP

# Local services
```

For each connection, the server only displays the address and port of the client, as
well as the requested command:

```
(localhost, 34638) requested command: ls -l
(localhost, 59288) requested command: ls -l
(localhost, 36936) requested command: cat /etc/services | wc -l
(localhost, 32788) requested command: cat /etc/services
(localhost, 47196) requested command: tail /etc/services
```

# Exercise 59-5

Suppose that we create two Internet domain datagram sockets, bound to specific addresses, and
connect the first socket to the second. What happens if we create a third datagram socket and
try to send (*sendto()*) a datagram via that socket to the first socket? Write a program to
determine the answer.

## Solution

I observed that the client received a false position its first message, which said it was
delivered successfully. The second message resulted in `ECONNREFUSED`.

To see this, compile as follows:

```bash
make inet_conn_dgram_sv
make inet_conn_dgram_cl
```

Then run the server in the foreground in one terminal window:

```bash
./inet_conn_dgram_sv
```

It will await for connections. On another terminal window, I ran the client as follows:

```bash
./inet_dgram_conn_cl hello world hi there
```

The output was:

```
Sent hello successfully
ERROR [ECONNREFUSED Connection refused] write() failed to send message : world
```

Meanwhile, the server did not show any output, indicating that it did not see any messages.

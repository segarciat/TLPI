# Exercise 46.2

Recode the sequence-number client-server application of Section 44.8 to use System V message
queues. Use a single message queue to transmit messages from both client to server and server
to client. Employ the conventions for message types described in Section 46.8.

## Solution

Sections 46.7 and 46.8 gave suggestions on how to use a single message queue to interchange messages
between a client and a server:

- The clients include their PID in their request to the server, and the server can use this
value as the *mtype* in its response to a client. This way, the client can filter the messages
according to their PID instead of reading it in FIFO order. For the server, the clients can
use an arbitrary PID that the server may never have, such as `1` (the PID of the *init* process).

- The server forks a child for each client request to avoid blocking and therefore delaying
ther request from other clients.

We can compile and run the server program as follows:

```bash
make sysvmq_seqnum_server
./sysmq_seqnum_server
```

The server creates the message queue and blocks as it anticipates requests from the client.
We can see the queue created by using the `ipcs` command:

```bash
ipcs
```

```
------ Message Queues --------
key        msqid      owner      perms      used-bytes   messages
0xffeeddcc 2          sgarciat   660        0            0
```

The server must run before the client to ensure the message queue is created. The server
assigns read and write permissions to the group in order to ensure the client can write to
and read from the message queue. The common header file used by the client and the server
contains the key for the message queue (here `0xffeeddcc`) which allows the client to open
the message queue. The client can then simply make a request, like so:

```bash
make sysvmq_seqnum_client
./sysvmq_seqnum_client 5
```

In the example above, I requested 5 sequence numbers, and since the requests begin with sequence
number 0, this is what the client sees:

```
0
```

The effect is that sequence numbers 0 through 4 are reserved for this client. Another client could
then request, say, 7 sequence numbers:

```bash
./sysmq_seqnum_client 7
```

```
5
```

This makes sense since the next sequence number available was 5, so this client is assigned
sequence numbers 5 through 11, and the next client will receive sequence numbers starting at
12.

Finally we can stop the server, and manually remove the message queue by specifying its
ID to the `ipcrm` command:

```bash
ipcrm -q 2
```

A limitation about this single queue approach is that once the queue fills, we may reach a
deadlock condition where neither the server nor any clients can read from or write to the
server. Also a client can intentionally decide to not read the messages, and the queue
may remain full, thus denying service to other clients. To fix this situation we would employ
a single queue per client.

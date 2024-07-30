# Exercise 52-2

Recode the sequence-number client-server application of Section 44.8 to use POSIX message queues.

## Solution

The minimum size for a message in a POSIX message queue is 128.

In recoding the application, I struggled with the fact that a message in a POSIX message queue
has to be a `char *`. When sending and receiving a message, either on the client or the server, I created
two related variables: a `char reqMsgBuf[]` buffer (or `rspMsgBuf`), and a `struct request req`
(or `struct response resp`). For example, on the server, the IPC calls look like so:

```c
mq_receive(serverMqd, reqMsgBuf, SERVER_MQ_MSGSZ, NULL);
memcpy(req, reqMsgBuf, sizeof(struct request));

/* ... */
mq_send(clientMqd, (char *) &resp, sizeof(struct response), 0);
```

The client contains a similar sequence of calls. The point is that a `char *` is needed in addition
to the actual structure describing the fields.

A further enhance I made is similar to Exercise 44-4, which removes the server's queue upon receipt
of a `SIGINT` or `SIGTERM`.

```bash
make pmsg_seqnum_server
make pmsg_seqnum_client
```

```bash
./pmsg_seqnum_server &
./pmsg_seqnum_client 11
./pmsg_seqnum_client 24
```

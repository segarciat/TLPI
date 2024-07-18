# Exercise 46.3

In the client-server application of Section 46.8, why does the client pass the identifier of
its message queue in the body of the message (in the *clientId* field), rather than in the
message type (*mtype*)?

## Solution

The *mtype* field of the message structure in *msgsnd()* must be an integer with a value greater
than 0. However, a message queue identifier is a System V IPC identifier, and this in turn may
be 0. This would constitute an illegal *mtype*, which could therefore cause *msgsnd()* to fail.

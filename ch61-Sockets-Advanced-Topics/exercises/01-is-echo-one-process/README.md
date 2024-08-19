# Exercise 61-1

Suppose that the program in Listing 61-2 (`is_echo_cl.c`) was modified so that, instead
of using *fork()* to create two processes that operate concurrently, it instead used just
one process that first copies its standard input to the socket and then read the server's
response. What problem might occur when running this client? (Look at Figure 58-8 on
page 1190.)

## Solution

To elaborate, `is_echo_cl.c` creates a stream socket connected to the *echo* server, then
it forks a child. From there, the parent reads data from standard input and writes it to
the socket. The child blocks as it waits to read the reply from the server, at which point
it displays it to sandard output.

Figure 58-8 illustrates that a TCP endpoint has a send buffer and a receive buffer. With
this in mind, the application would have responsible for the following:

1. Read data on standard input.
2. Send data to the peer TCP endpoint by placing it on the send buffer.
3. Receive data from the peer TCP endpoint by reading it from the receive buffer.
4. Write the received data to standard output.

The client might read large amounts of data from standard input. Suppose, for example,
that the send and receive buffer had a capacity of 1K each. Furthermore, suppose
that our application is designed to read a maximum of 8K bytes at a time, and that
capacity is reached in one read operation. The peer TCP would read 1K from our TCP,
and then place 1K as a response in oru receive buffer. Then we write 1K more to the
send buffer, and the peer TCP again reads it and tries to write it to our receive
buffer. Meanwhile, we try to write more data to the buffer, and we've reached a situation
where the server is blocked on write because our receive buffer is full, and our client
is blocked on write because our send buffer is full, too.

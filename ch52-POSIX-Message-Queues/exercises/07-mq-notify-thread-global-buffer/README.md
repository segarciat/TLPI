# Exercise 52-7

In Listing 52-7, could *buffer* be made a global variable and its memory allocated just once
(in the main program)? Explain your answer.

## Solution

This might seem safe because the *mq_msgsize* attribute of a POSIX message queue cannot be
changed once it is created. However, allocating the buffer in *main()* is not safe because
it creates a race condition. After *threadFunc()* calls `notifySetup(mqdp)` to re-register
for message queue notifications, it's possible the queue becomes drained and a new thread
is spawned that executes concurrently with the first one. At this point, they may both
be filling *buffer* through calls to *mq_receive()*, leading to unexpected results.

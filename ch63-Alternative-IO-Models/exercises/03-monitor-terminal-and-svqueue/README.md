# Exercise 63-3

Section 63.5 noted that *select()* can't be used to wait on both signals and file
descriptors, and described a solution using signal handlers and a pipe. A related
problem exists when a program needs to wait for input on both a file descriptor and
a System V message queue (since System V message queues don't use file descriptors).
One solution is to fork a separate child process that copies each message from the
queue to a pipe included among the file descriptors monitored by the parent. Write
a program that uses this scheme with *select()* to monitor input from both the
terminal and a message queue.

## Solution

I created a program that creates a System V Message queue using the `IPC_PRIVATE`
flag in order to forego the creation of a unique key. The program then displays the
ID of the System V message queue on standard output. Next, the program creates
a pipe, and forks a child. At this point, the child inherits the pipe file descriptors,
and the ID of the messagequeue, for which it has permissions.

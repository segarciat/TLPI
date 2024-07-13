# Exercise 44.5

The server in Listing 44-7 (`fifo_seqnum_server.c`) performs a second `O_WRONLY` open of the
FIFO so that it never sees end-of-file when reading from the reading descriptor (*serverFd*).
Instead of doing this, an alternative approach could be tried: whenever the server sees end-of-file
on the reading descriptor, it closes the descriptor, and then once more opens the FIFO for reading.
(This open would block until the next client opened the FIFO for writing.) What is wrong with this
approach?

## Solution

Under the current approach, the fact that the server holds a FIFO descriptor open for writing
means that the server blocks when it attempts to read from the FIFO if no client has written to it.
That is, the process is suspended while it idly waits for a client to make a request.
If the server did not have this second `O_WRONLY` descriptor open, then so long as no client request
is pending, `read()` will *immediately* return end-of-file. The consequence is that it will
repeatedly call `read()` and `open()`, wasting precious CPU cycles. Moreover, there is a race
condition introduced by this approach. If a client manages to open the server FIFO while the server
still has the descriptor open, then it could be preempted by the kernel, during which the
server could close the file descriptor. The kernel could then suspend the server (perhaps its time
slice runs out), and then the client will attempt to write to the file descriptor, and receive
a `SIGPIPE` because the server has not yet re-opened the read end. By default the signal will
kill the client process. Depending on the timing, another possibility is that the server might
determine that no input is yet available on the FIFO (i.e., `read()` returns end-of-file), and
the server is preempted before reopening the descriptor. At that point, the client may both
open the FIFO and successfully write data to it. But when the server is scheduled again, it
will just close the FIFO, and never know it received a request; in effect, the request is
discarded. Now the client blocks, waiting for a response that will never arrive.

# Exercise 52-1

Modify the program in Listing 52-5 (`pmsg_receive.c`) to accept a timeout (a relative number of seconds)
on the command line, and use *mq_timedreceive()* instead of *mq_receive()*.

## Solution

In this program, I employed POSIX clocks, so it is necessary to link against *librt* by
providing the `-lrt` option when compiling. The appropriate flags has been added to the `Makefile`.
I have also included other code listings from the book to create a POSIX message queue and send
a message in order to verify the functionality.

The modified `pmsg_receive` program requires the name of a message queue as its first argument,
and optionally accepts a value nonnegative seconds and nanoseconds value to use as a timeout
in the second and the third arguments, respectively. One can provide seconds and omit the nanoseconds,
but if we wish to specify nanoseconds, then the seconds are required.

```bash
make pmsg_create
make pmsg_send
make pmsg_receive

# Create a message queue with name mq
./pmsg_create /mq

# Blocks because queue is empty
./pmsg_receive
# CTRL+C to interrupt with SIGINT

# Provide 3 second timeout, which blocks for 3 seconds and exits
./pmsg_receive 3 &

# Provide 10 second timeout and run in the background
./pmsg_receive 10 &

# Send a message
./pmsg_send /mq "hello world"
```

If the `pmsg_send` call happens sufficiently quickly after the `pmsg_receive` with the 10 second timeout,
then the process executing `pmsg_receive` will unblock, display to standard out from the background
(causing the output to be intermingled with the terminal prompt) and to exit.

Sample output follows:

```
Read 11 bytes; priority = 0
hello world
[1]+  Done                    ./pmsg_receive /mq 10
```

Section 52.7 explains that on Linux, POSIX IPC objects are implemented as files in virtual file
systems, which can be listed with `ls` and removed with `rm`. For message queues, we must mount
the message queue file system with a command of the form:

```
mount -t mqueue source target
```

where *source* is any name (typically *none*) and *target* is the mount point.
In my case, the message queue file system already exists so I did not have to do this prior. In any
case, once you're doing with this exercise, you can delete the message queue:

```bash
ls -l /dev/mqueue
rm /dev/mqueue/mq
```

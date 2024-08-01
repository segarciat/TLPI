# Exercise 52-5

Modify the program in Listing 52-6 (`mq_notify_sig.c`) to demonstrate that message notification established
by *mq_notify()* occurs just once. This can be done by removing the *mq_notify()* call inside the `for`
loop.

## Solution

The only change I made was to delete the lines that took care of re-registering the process
for notification in the `for` loop prior to draining the queue.

I also copied the source files from the other code listings in order to create the queue and
send messages to it from the command-line.

```bash
# Compile programs
make pmsg_create
make pmsg_send
make mq_notify_sig

# Create message queue
./pmsg_create -cx /mq_notify_52_5

# Verify it is present in the mqueue filesystem
ls -l /dev/mqueue | grep mq_notify

# Start mq_notify_sig in the background
./mq_notify_sig /mq_notify_52_5 &
# Output: [1] 8509

# Verify notification setup
# Output: QSIZE:0          NOTIFY:0     SIGNO:10    NOTIFY_PID:8509

# Send a message to the queue and observe the output of mq_notify_sig
./pmsg_send /mq_notify_52_5 hello
# Output: Read 5 bytes

# Send another message, and notice it does not show anything
./pmsg_send /mq_notify_52_5 goodbye
# No output

# Verify contents of queue to see if there is indeed a message available
ls /dev/mqueue/mq_notify_52_5
# Output: QSIZE:7          NOTIFY:0     SIGNO:0     NOTIFY_PID:0

# Kill the background job
kill %1

# Remove the message queue (or let it persist until system restart)
rm /dev/mqueue/mq_notify_52_5
``` 

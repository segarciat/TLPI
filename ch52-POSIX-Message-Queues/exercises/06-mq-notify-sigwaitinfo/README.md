# Exercise 52-6

Replace the use of a signal handler in Listing 52-6 (`mq_notify_sig.c`) with the use
of *sigwaitinfo()*. Upon return from *sigwaitinfo()*, display the values in the returned
*siginfo_t* structure. How could the program obtain the message queue descriptor in the
*siginfo_t* structure returned by *sigwaitinfo()*?

## Solution

The *sigsuspend()* and *sigwaitinfo()* system calls share in common that they are both
used to synchronously accept the delivery of a signal. As mentioned in the instructions,
*sigsuspend()* requires us to set up a signal handler for the signals we wish to accept
because the call suspends process execution until it is interrupted (by a signal handler).
The *sigwaitinfo()* requires that we add the signals we wish to accept to the process
signal mask, and then provide it with this same list. It suspends the process until
any of those signals become pending, at which point execution resumes.

To obtain the message queue descriptor in the `siginfo_t` structure returned by `sigwaitinfo()`,
we can assign the a pointer to the message queue descriptor to the `sigev_value.sigval_ptr`
field in the `struct sigevent` structure which is passed as the *notification* parameter to
*mq_notify()*.

```bash
make pmsg_create
make pmsg_send
make mq_notify_sig

# Create a message queue
./pmsg_create /mq_notify_52_6

# Start the program in the background
./mq_notify_sig /mq_notify_52_6

# Send a message
./pmsg_send /mq_notify_52_6 hello
```

Sample output is shown below:

```
Message queue descriptor: 3
Number Code PID          UID          Value
10     -3   11784        1000         3
Read 5 bytes
```

The UID and PID corresponding to the real UID of the sending process as well as its PID. `Number`
stands for the signal number, which is `10` for `SIGUSR1`, the notification signal used by the
program. `Code` refers to the signal code, and `-3` corresponds to `SI_MESGQ` (see page 441, Table 21-2).
As you can see, the Value is precisely the message queue descriptor (which in Linux is an integer,
since it is implemented as a file descriptor).

```bash
# Signal background process to terminate
kill %1

# Delete the unneeded queue
rm /dev/mqueue/mq_notify_52_6
```

# Exercise 46.1

Experiment with the programs in Listing 46-1 (`svmsg_create.c`), Listing 46-2 (`svmsg_send.c`),
and Listing 46-3 (`svmsg_receive.c`) to confirm your understanding of the *msgget()*, *msgsnd()*,
and *msgrcv()* system calls.

## Solution

After fetching the source files, I copied over my `Makefile.template` and
appended the following lines:

```makefile
svmsg_create:
svmsg_send:
svmsg_receive:
```

Then I compiled the programs with:

```bash
make svmsg_create
make svmsg_send
make svmsg_receive
```

I began by creating a System V message queue with:

```bash
./svmsg_create -p -c
```

To create a System V IPC object, we need a key. The `-c` option is used to request that the
`IPC_CREAT` flag is set, therefore ensuring a queue is created if no existing queue matches
the key we provided. Since we also specified `-p` instead of providing a key, a unique key is
generated in our behalf, and thus a new queue is created. The program in my case outputs `0`:

```
0
```

The output value is the identifier for the System V IPC object (i.e., the message queue). This
is similar to a file descriptor for a file. In the following command I yet another queue, but
this time specify a key myself:

```bash
./svmsg_create -c -k 7
```

```
1
```

As can be seen from the output, a queue was created with identifier `1`. The key can be any 32-bit
integer, and here I provided `7`. The queue did not exist so it was created. Running the same command
returns the same identifier for the same queue, meaning a queue is not created again:

```bash
./svmsg_create -c -k 7
```

```
1
```

However, if we also provide `-x`, then the `IPC_EXCL` flag is set, which allows us to guarantee that
a new queue is created if no existing queue has the same key. If a queue with the key we provided
exists, then our call fails:

```bash
./svmsg -c -k 7 -x
```

```
ERROR [EEXIST File exists] msgget
```

We can create a new one:

```bash
./svmsg -c -k 8 -x
```

```
2
```

Yet another alternative for the key of the message queue is to provide a filename. Then the `ftok()`
function is employed to generate a key by using the two least significant bytes of the file's i-node
number, the file's minor device number, and a *proj* value, which is an 1 byte integer (by default,
the program uses  `1`,and does not allow us to specify this as a command-line argument).

```bash
./svmsg_create -c -f README.md
```

```
3
```

The `ftok()` algorithm is highly likely to producea unique key, but it is not unique. So it is generally
preferable to create the key with `IPC_PRIVATE` whenever possible. The caveat is that we do not know
the key, though we do obtain identifier after the function call, so we need a way to communicate this
identifier with other processes that may need it.

Having created the queue, we can use the *ipcs(1)* command to list the existing System V IPC objects.
The following is partial output from the command:

```
----- Message Queues --------
key        msqid      owner      perms      used-bytes   messages
0x00000000 0          sgarciat   600        0            0
0x00000007 1          sgarciat   600        0            0
0x00000008 2          sgarciat   600        0            0
0x010561b9 3          sgarciat   600        0            0
```

By default the `svmsg_create` program assigns the permissions `600`, which is an octal value that means
read and write permission for the owner and no permissions for anyone else. It's possible to
specify permissions in octal form to the `svmsg_create` program by passing it as the last argument,
but I chose to omit this.

Now we can use the `svmsg_send` program to send a message to the message queue:

```bash
./svmsg_send 0 100 "Hello"
./svmsg_send 0 300 "See you soon!"
./svmsg_send 0 100 "Greetings"
./svmsg_send 0 200 "How are you?"
./svmsg_send 0 300 "Goodbye"
./svmsg_send 0 100 "Hi"
./svmsg_send 0 200 "What are you up to?"
```

The first argument, `0`, is the message queue identifier, and in this case it corresponds to
the very first queue I created. The second integer is the message type, which can be any integer.
The purpose of this integer is to receive messages from the queue by message type, instead of
in the FIFO order. The last argument is the message actually sent. For example, we could receive
the `See you soon` message first, by specifying the message type:

```bash
./svmsg_receive -t 300 0
```

```
Received: type=300; length=14; body=See you soon!
```

Now this message will be consumed, so if we ran the same command, we would see `Goodbye`. We could
also specify the maximum number of bytes we wish to accept from the message queue by providing a
second positional argument to `svmsg_receive`. If the message length exceeds that, then we actually
get an error `E2BIG`. In the following I specify I am willing to accept 3 bytes, which exceeds
the length of `Goodbye`:

```bash
./svmsg_receive -t 300 0 3
```

```
ERROR [E2BIG Argument list too long] msgrcv
```

When this happens, the message is not consumed, so it is still available in the queue. We could
alternatively specify `-e` so that the `MSG_NOERROR` flag is set. In this case, there is no error,
and the message is truncated to meet our length restriction, but the excess data is lost:

```bash
./svmsg_receive -e -t 300 -n 0 3
```

```
Received: type=300; length=3; body=Goo
```

If we were to attempt to receive another message of type 300, our process would block because no more
message of that type would match. It unblocks and receives the message as soon as one is available
on the queue that matches that. The type can be used in other ways. For example, a message type of `0`
means we receive the first message available. A negative message type means we treat the message queue
as a priority queue.

# Exercise 59-1

When reading large quantities of data, the *readline()* function shown in Listing 59-1
is inefficient, since a system call is required to reach each character. A more efficient
interface would read a block of characters into a buffer and extract a line at a time from
this buffer. Such an interface might consist of two functions. The first of these functions,
which might be called *readLineBufInit(fd, &rlbuf)*, initializes the bookkeeping data
structure pointed to by *rlbuf*. This structure includes space for a data buffer, the size
of that buffer, and a pointer to the next "unread" character in that buffer. It also includes
a copy of the file descriptor given in the argument *fd*. The second function, *readLineBuf(&rlbuf)*,
returns the next line from the buffer associated with *rlbuf*. If required, this function reads
a further block of data from the file descriptor saved in *rlbuf*. Implement these two functions.
Modify the programs in Listing 59-6 (`is_seqnum_sv.c`) and Listing 59-7 (`is_seqnum_cl.c`) to
use these functions.

## Solution

### Approach to Implementing *rlbuf*

I developed an API to adhere to the specification in this exercise. The header `rlbuf.h` consists
of a structure declaration and two function declarations:

```bash
struct rlbuf {
    int fd;
    char buf[_RLBUF_SIZE];
    size_t size;
    char* p;
};

int readLineBufInit(int fd, struct rlbuf* rlb);
ssize_t readLineBuf(struct rlbuf* rlb, void *buffer, size_t n);
```

The source file `rlbuf.c` implements this API.

The *readLineBufInit()* function does not read any bytes. Therefore it simply sets the `fd` to the
given value, the size to `0` (since no bytes have been read), and the `p` field to point to the `buf` field.

The *readLineBuf()* begins by checking whether pointer *p* to the next unread character has reached beyond
the end of the buffer. If so the buffer is refilled and the *size* is set to the number of bytes read.
The *read()* operation is restarted in the event that it is interrupted while the buffer is being refilled.

Bytes are read into buffer subject to the following conditions:

1. A newline character has been read.
2. No more than `n-1` bytes are read into the user's buffer.
3. EOF is encountered.

It's possible for `n-1` characters to be reached before either EOF or a newline are encountered.
In this case, the function continues to read data but does not store it in the buffer. This is
intentional and consistent with *readLine()* in Listing 59-1.

### Modifications to `is_seqnum` Programs

The change was simple. I replaced the call to *readLine()* with a call to *readLineBuf()*.

### Compiling and Running

To compile the program, run:

```bash
make is_seqnum_sv
make is_seqnum_cl
```

I tried out the program by running the same interactions as on page 1223:

```bash
./is_seqnum_sv &
# [1] 18045

./is_seqnum_cl localhost
# Connection from (localhost, 39938)
# Sequence number: 0

./is_seqnum_cl localhost 10
# Connection from (localhost, 39952)
# Sequence number: 1

./is_seqnum_cl localhost
# Connection from (localhost, 38444)
# Sequence number: 11
```

So far, all interactions match the expected result. Next, an interaction with `telnet` as demonstrated
in Section 59.11:

```bash
telnet localhost 50000
```

After the connection is made, I entered `1` and received `12` as the sequence number:

```
Trying 127.0.0.1...
Connected to localhost.
Escape character is '^]'.
Connection from (localhost, 42376)
1
12
Connection closed by foreign host.
```

Finally terminate the server:

```bash
kill 18045
```

# Exercise 52-3

Rewrite the file-server application of Section 46.8 to use POSIX message queues instead of System V
message queues.

## Solution

A couple considerations came up while recoding this application. For one, since each call to *read()*
involved a different number of bytes, rather than always transfer `sizeof(struct responseMsg)` bytes
in each message from the server to the client, I used `offsetof()` as well as `strlen()` and the
return value of `read()` to ensure a proper number of bytes were written. This also meant I did
not need to zero out the array each time I sent a new message.

That being said, because the number of bytes received include the header byte and any padding adder
by the compiler for alignment `struct responseMsg`, the byte count displayed by the client may be
larger than the actual number of bytes in the file.

```bash
make pmsg_file_server
make pmsg_file_client
./pmsg_file_server &
./pmsg_file_client README.md
# kill %1 if it has job ID 1 as a background process, or...
fg
# CTRL+C to terminate server with SIGINT
```

# Exercise 59-2

Modify the programs in Listing 59-6 (`is_seqnum_sv.c`) and Listing 59-7 (`is_seqnum_cl.c`)
to use the *inetListen()* and *inetConnect()* functions provided in Listing 59-5 (`inet_sockets.c`).

## Solution

I successfully removed all the boilerplate by leveraging *inetListen()* and *inetConnect()*.

Compile and run with:

```bash
make is_seqnum_sv
make is_seqnum_cl
```

Run the server in the background and make requests from the client:

```bash
./is_seqnum_sv &
# [1] 25015

./is_seqnum_cl localhost
# Connection from (localhost, 33380)
# Sequence number: 0

./is_seqnum_cl localhost 10
# Connection from (localhost, 53650)
# Sequence number: 1

./is_seqnum_cl localhost
# Connection from (localhost, 53664)
# Sequence number: 11

telnet localhost 50000
# Trying 127.0.0.1...
# Connected to localhost.
# Escape character is '^]'.
# Connection from (localhost, 50358)
# 1
# 12
Connection closed by foreign host.
```

In the last interaction using `telnet`, I requestes `1` sequence number, and received `12` as the output,
as expected.

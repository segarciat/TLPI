# Exercise 57-3

Reimplement the sequence-number server and client of Section 44.8 using UNIX
domain stream sockets.

## Solution

The application is the same as in the 44.8, so I will skip explaining what
it does.

Compile as follows:

```bash
make sockun_seqnum_sv
make sockun_seqnum_cl
```

Run the server in the background:

```bash
./sockun_seqnum_sv &
```

```
[1] 32443
```

Then request a sequence number by using the client and specifying a
positive integer as a command-line argument:

```bash
./sockun_seqnum_cl 5
```

The output will be `0` because that is where sequence number begin to
get assigned:

```
0
```

Then another request should yield 5:

```bash
./sockun_seqnum_cl 8
```

```
5
```

The next request should yield `13` (`5 + 8`) as a result, and so on.
Clean up by terminating the server and removing the UNIX socket pathname:

```bash
kill 32443

# See sockun_seqnum.h
rm /tmp/sockun_seqnum
```

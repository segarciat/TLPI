# Exercise 61-4

Write a program that uses *getsockname()* to show that, if we call *listen()* on a TCP socket
without first calling *bind()*, the socket is assigned  an ephemeral port number.

## Solution

To achieve this task, my program begins by using *getaddrinfo()* ,specifying port number `50000`.
After the program calls *socket()* and *listen()* successfully to create a passive socket, it calls
*getsockname()* to find out the socket address it was assigned. Finally, using *getnameinfo()*,
it fetches the host and service string, and displays it on standard out.

Compile and run as follows:

```bash
gcc -Wall -o sock_listen_ephem sock_listen_ephem.c
./sock_listen_ephem
```

The output shows thatthe port chosen is indeed an ephemeral port, rather than the one
we passed to *getaddrinfo()*:

```
We specified port 50000 and were assigned: (0.0.0.0, 45449)
```


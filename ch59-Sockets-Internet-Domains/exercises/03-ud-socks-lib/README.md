# Exercise 59-3

Write a UNIX domain sockets library with an API similar to the Internet domain sockets
library shown in Section 59.12. Rewrite the programs in Listing 57-3 (`us_xfr_sv.c`,
on page 1168) and Listing 57-4 (`us_xfr_cl.c`, on page 1169) to use this library.

## Solution

Similar to `inet_sockets.h`, the `ud_sockets.h` has three main functions:

```c

```

Begin by compiling the client and server:

```bash
make us_xfr_sv
make us_xfr_cl
```

Then we can test the implementation by running the same shell session as in Section 57.2,
page 1170:

```bash
# Run in the background and direct output to file "b"
./us_xfr_sv > b &
# [1] 207306

# Verify socket file was successfully created
ls -lF /tmp/us_xfr

# Create input test file
cat *.c > a

# Run client with test input
./us_xfr_cl < a

# Terminate server
kill 207306

# Verify files contain the same content
diff a b
```

The last command yielded no output, indicating the files are indeed the same.

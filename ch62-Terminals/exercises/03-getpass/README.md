# Exercise 62-3

Implement the *getpass()* function described in Section 8.5. (The *getpass()* function
can obtain a file descriptor for the controlling terminal by opening `/dev/tty`.)

## Solution

According to Section 8.5, get *getpass()* function first disables echoing and all
processing of terminal special characters (such as the *interrupt* character).
Then it prints the string pointed to by *prompt* and reads a line of input,
returning the null-terminated input string with the trailing newline stripped as
its function result. Before returning, it restores terminal settings to their original
state.

Although disabling canonical mode disables processing of all special characters,
I did not go along with this before it disable special processing of EOF as well.
From experimenting with the glibc *getpass()* implementation, I noticed that special
processing for EOF remained in effect. Therefore, instead, I set the values of all
entries in the `c_cc` array field (except `VEOF`) of the `termios` structure to the
value returned by *fpathconf(fd, _PC_VDISABLE)*.

Moreover, I changed the function signature so instead of returning a statically
allocated string, I have the user provide a large enough buffer. The proper size
is advertised in `getpass.h`.

To try out the accompanying main program, run:

```bash
gcc -Wall -o main main.c getpass.c
./main
```

You will be prompted:

```
Enter secret: 
```

If we immediately typed the EOF character (usually *Ctrl-D*), then the process unblocks
and *_getpass()* returns (consistent with the glibc implementation). If we typed, say, `hel`
followed by EOF (usually pressing `Ctrl-D`), then the process remains blocked waiting for
input (consistent with the glibc implementation). If we typed EOF or a newline character,
the function returns (again, consistent with the glibc implementation).

For example, typing `hel`CTRL-D`lo` outputs `hello`.

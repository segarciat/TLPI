# Exercise 63-8

Modify the program in Listing 63-3 (`demo_sigio.c`)  to use a realtime signal instead of
`SIGIO`. Modify the signal handler to accept a *siginfo_t* argument and display the values
of the *si_fd* and *si_code* fields of this structure.

## Solution

As discussed in Section 63.3.2, we can use the *fcntl()* `F_SETSIG` operation to specify
an alternative signal that should be delivered instead of `SIGIO` when I/O is possible.
Moreover, it requires that the `_GNU_SOURCE` feature test macro be defined in order to
be able to obtain the constant from `<fcntl.h>`.

In this case, I chose the 4th real time signal (`SIGRTMIN + 3`), where `SIGRTMIN` is
available through `<signal.h>`. In 22.8, Kerrisk advises not to refer to `SIGRTMIN`
or `SIGRTMAX` because SUSv3 does not require them to be integers; they may be defined
as follows. Therefore, instead of storing a constant `SIGRTMIN + 3`, I stored the signal
offset as a constant.

To install a signal handler that is capable of catching a realtime signal, I first had
to set `SA_SIGINFO` in the *sa_flags* field of the `struct sigaction` structure. I also
needed to set the *sa_sigaction* field of the `struct sigaction` structure, as opposed
to *sa_handler*. Moreover, I updated the function declaration to accept the necessary,
fields, including a *siginfo_t* structure:

```c
void sigioHandler(int sig, siginfo_t *si, void *ucontext);
```

To display the *si_fd* and *si_code* fields, I used *printf()*, which is in general
a non-async-signal-safe function, but is appropriate for this exercise's purpose.

To compile and run:

```bash
make
./demo_sigio
```

After typing the characters, `a`, `b`, and `#`, we see:

```
si_fd=0, si_code=1 (POLL_IN)
cnt=14; read a
si_fd=0, si_code=1 (POLL_IN)
cnt=26; read b
si_fd=0, si_code=1 (POLL_IN)
cnt=52; read #
```

The `si_fd=0` follows from the fact that we are monitoring `STDIN_FILENO`, is file descriptor
`0`. Meanwhile, `si_code=1` is a code indicating an event is possible. In particular, this
happens to be the value of `POLL_IN`, meaning input is available. As in Listing 63-3, the
program ends upon reading `#`.

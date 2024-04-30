# Exercise 22.1

Section 22.2 noted that if a stopped process that has established a handler for
and blocked `SIGCONT` is later resumed as a consequence of receiving a `SIGONCT`,
the the handler is invoked only when `SIGCONT` is unblocked. Write a program to
verify this. Recall that a process can be stopped by typing the terminal *suspend*
character (usually *Control-Z*) and can be sent a `SIGCONT` signal using the command
*kill -CONT* (or implicitly, using the shell `fg` command).

## Solution

My program begins by establishing a signal handler for `SIGCONT` which uses the
non-async-signal-safe function `printf()` to display `Handled SIGCONT` when the
`SIGCONT` signal is delivered to the process. It then adds `SIGCONT` to the process
signal mask and displays a message saying this has occurrs. Then, it begins a 10 second
countdown, after which it unblocks the signal, prints a message from `main()`, and exits.
If a `SIGCONT` signal is sent to the process in the period between the blocked and unblocked
message, the program continues but the handler is not invoked but is added to the set
of pending signals and is invoked as soon `sigprocmask()` unblocks it (and possibly
before it even returns).

To see this in action, take these steps:

1. Compile with `gcc -o verify_sigcont verify_sigcont.c`.

2. Run `./verify_sigcont`. Note that it displays the process ID, so note it down.

3. After seeing the message `Blcoking and handling SIGCONT`, press *Control+Z* to send a
job-control *stop* signal before the counter reaches 0. This stops (suspends) the process
running in the foreground (should be `verify_sigcont`).

4. In the same or a different terminal, use the process ID you noted down (or use the `ps xa`
command to find the PID for the process corresponding to `verify_sigcont`) and run
`kill -s SIGCONT <PID>`, where you replaced `<PID>` with the process ID.

5. The count will continue, but when the counter reaches 0, the `Handled SIGCONT` message
will display from the handler because `SIGCONT` has been removed from the process signal mask.


```bash
gcc -o verify_sigcont verify_sigcont.c
./verify_sigcont
```

Then the output begins:

```
PID: 37373
Blocking and handling SIGCONT
Unblocking SIGCONT in: 10
9
8
^Z # pressed Control+Z
[1]+  Stopped                 ./verify_sigcont
```

Then run

```bash
kill -s SIGCONT 37373
```

The output continues, intermixed with the shell prompt because it's not running as a foreground
process:

```
7
sgarciat@sgarciat-TUF-FX505GT:~/code/repos/TLPI/ch22-Signals-Advanced-Features/exercises/01-sigcont$ 6
5
4
3
2
1
0
Handled SIGCONT
Exiting
^C # Press Control+C to interrupt
[1]+  Done                    ./verify_sigcont
```

# Exercise 34.7

Write a program to verify that if one of the signals `SIGTTIN`, `SIGTTOU`, or `SIGTSTP` is
sent to a member of an orphaned process group, then the signal is discarded (i.e., has
no effect) if it would stop the process (i.e., the disposition is `SIG_DFL`), but is
delivered if a handler is installed for the signal.

## Solution

My program uses `fork()` to create a child. The parent exits immediately and the child sleeps briefly.
This causes the child to be orphaned and to be in an orphaned process group. Then the child
uses `raise()` to deliver itself a `SIGTTOU`, `SIGTTIN`, and `SIGTSTP` signal.

If *any* command-line argument is provided, the handler for `SIGTTOU`, `SIGTTIN`, and `SIGTSTP`
is installed. Thus if we run the program as follows

```bash
./a.out 1 &
```

then we see the following output:

```
$ Parent exiting
Child (49950) will sleep
Child resumed, sending signals
Handler caught signal 20 (Stopped)
Handler caught signal 21 (Stopped (tty input))
Handler caught signal 22 (Stopped (tty output))
Child exiting

[1]+  Done                    ./a.out 1
```

However, if we do not provide a command-line argument, then the handler is not installed,
which means that the default action would take effect. The default action for these is
to stop the process. However, this does not occur because the signals are discarded, due
to the fact that the process group is orphaned. Therefore, if we run the command as follows:

```bash
./a.out &
```

it will output

```
$ Parent exiting
Child (50000) will sleep
Child resumed, sending signals
Child exiting

[1]+  Done                    ./a.out
```

and if we run `ps xa | grep ./a.out`, there is no trace of the process, because it
has exited (it was not stopped). If we were to introduce a sleep in the parent,
such as `sleep(10)` so that the child could proceed to signal itself before it became
orphaned, we would see that the child is stopped:

```bash
ps xa
```

```
  49999 pts/1    S      0:00 ./a.out
  50000 pts/1    T      0:00 ./a.out
```

When the parent wakes, the child and its process group become orphaned, which causes
the kernel to send a `SIGCONT`, thus continuing the child. Moreover, its attempt 
to print text fails with `EIO`, so no text is shown (as described in page 730, Section 34.7).

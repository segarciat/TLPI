# Exercise 34.6

Write a program to verify that when a process in an orphaned process group attempts to `read()`
from the controlling terminal, the `read()` fails with `EIO`.

## Solution

A process group becomes orphaned if none of its member processes has a parent in a different
process group in the same session. My program uses `fork()` to create a child process. At
that point, the parent and child belong to the same process group, with the parent's process
group being the shell process (which is in a different process group within the same session).
The parent exits immediately, but the child calls `read()`. After attempting to
provide input, the `read()` call should fail and the reason is displayed before the child exits.

To achieve this effect, the program must be run in the background:

```bash
gcc orphan_pgrp_read.c
./a.out &
```

The following output is shown:

```
$ Parent exiting
Child going to attempt to read input
As expected, program ended due to EIO: Input/output error

[1]+  Done                    ./a.out
```

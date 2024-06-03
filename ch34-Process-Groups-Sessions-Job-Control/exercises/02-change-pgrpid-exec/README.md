# Exercise 34.2

Write a program to verify that a parent process can change the process group ID of
one of its children before the child performs an *exec()*, but not afterward.

## Solution

My program begins by blocking `SIGUSR1` because I use it an IPC mechanism between
the parent and child, each of which may use `sigwaitinfo()` to synchronously accept
the signal. After the `fork()`, both the child and parent display their relevant
IDs, such as their process ID, parent process ID, process group ID, and session ID.
Then the program proceeds depending on whether command-line arguments were provided.

By default, meaning if no command-line arguments were provided, the parent suspends
using `sigwaitinfo()` to allow the child to display its ID information. The child
displays its IDS, informs the parent to proceed using `kill()`, and uses `sigwaitinfo()`
to suspend. The parent then resumes, uses `setpgid()` to set its child's process group
ID, and then exits. Finally, the child resumes, displays its IDs once again, and
calls `execlp()` to run `sleep 5`. 

If a command-line argument is provided, then the child does not suspend after
displaying its IDs. Instead, it immediately proceeds to call `execlp()` to run
the `sleep 5` program. On the other hand, the parent does not suspend in anticipation
of `SIGUSR1` either; it instead calls `sleep(1)` to suspend for a short amount of
time in order to allow the child to *exec()* a program (of course this is imperfect,
but it's sufficient for this purpose). Then it attempts to call `setpgid()` to set
the child's process group ID, which fails, because the child has already called
*exec()* (and presumably, it is still alive but suspended due to `sleep 5`).

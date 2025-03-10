# Exercise 12-01

Write a program that lists the process ID and command name for all processes being run by
the user named in the program's command-line argument. (You may find the `userIdFromName()`
function from Listing 8-1, on page 159, useful). This can be done by inspecting the *Name:*
and *Uid:* lines of all of the `/proc/PID/status` files on the system. Walking through all
of the `/proc/PID` directories on the system requires the use of `readdir(3)`, which is
described in Section 18.8. Make sure your program correctly handles the possibility that
a `/proc/PID` directory disappears between the time that the program determines that the
directory exists and the time that it tries to open the corresponding `/proc/PID/status`
file.

## Solution

To build and run:

```bash
make
./pslist root
```

Here's a trimmed version of the sampled output:

```
PID: 1		Command: systemd
PID: 2		Command: kthreadd
PID: 3		Command: pool_workqueue_release
...
PID: 16753		Command: kworker/u32:2
```

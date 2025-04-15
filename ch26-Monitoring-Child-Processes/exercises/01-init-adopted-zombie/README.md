# Exercise 26.1

Write a program to verify that when a child's parent terminates, a call to `getppid()` returns
1 (the process ID of `init`).

## Solution

The program `init_adopted_zombie.c` is my implementation:

```bash
make
./init_adopted_zombie
```

The output is:

```
Parent (28445): Sleeping...
Child (28446): Parent has pid 28445. Sleeping...
Parent (28445): Exiting
sgarciat@sgarciat-TUF-FX505GT:~/code/repos/TLPI/ch26-Monitoring-Child-Processes/exercises/01-init-adopted-zombie$ Child (28446): Awake! Parent has pid 1853
Found! Name:	systemd

^C # control-C
```

The output is intermingled with the shell prompt because once the parent terminates, control
returns to the shell process. The child process, which terminates one second later, then
outputs its PID and that of its new parent.

Note that parent is initially a process with PID of 28445. The parent sleeps for a second, allowing
the child to output a message displaying its current parent's PID; it is the process with PID 28445,
as expected. After a second, the parent wakes up and exits, while the child is still alive. When the
child wakes up, it displays its new parent's PID, which is 1853, not 1.

The [TLPI errata](https://man7.org/tlpi/errata/index.html) explains the following:

>Linux 3.4 added the `PR_SET_CHILD_SUBREAPER` `prctl()` operation, which allows a "service manager" process to mark itself as a sort of 'sub-init', so that it adopts all orphaned processes created by the services it starts. All `SIGCHLD` signals will be delivered to the service manager. There is a corresponding `PR_GET_CHILD_SUBREAPER` `prctl()` operation.

>More recent *init* frameworks employ the `PR_SET_CHILD_SUBREAPER` feature to create a dedicated subreaper process that becomes the parent of all orphaned children. On such frameworks, which include `systemd` (dependent on start-up options) and (the now obsolete) upstart, `getppid()` will report the new parent PID as the PID of the subreaper process (i.e., a value other than 1) when a process becomes orphaned.

I proceeded to verify that this corresponding to the process ID for `systemd` by looking at the `/proc`
file system. The output above shows that `systemd` is the the subreaper.

# Exercise 34.1

Suppose a parent process performs the following steps

```c
/* Calls fork() to create a number of child processes, each of
   which remains in the same process group as the parent */

/* Sometime later... */
signal(SIGUSR1, SIG_IGN);		/* Parent makes itself immune to SIGUSR1 */

killpg(getpgrp(), SIGUSR1);		/* Send signal to children created earlier */
```

What problem might be encountered with this application design? (Consider shell pipelines.)
How could this problem be avoided?

## Solution

The default action for `SIGUSR1` is to terminate a process (see Table 20.1 on page 396).
The problem is that `killpg()` sends the signal to *all* processes in the specified
process group. If the caller and its children are the only processes in the process group,
then this will achieve the (probably) intended effect of terminating the children of the
parent process (or calling the installed handler in them). However, if the program is run
as part of a pipeline, then all of the processes in the pipeline are in the same process group,
and unless they have changed the disposition for `SIGUSR1` or blocked the signal, they will
be terminated.

To avoid this, prior to calling `killpg()`, the caller could use `setsid()` to create
a new session and become the session and process group leader there. However, as described
in Section 34.3, page 705, if the calling process is a process group leader, then
the call to `setsid()` will fail witH `EPERM`. Therefore, to avoid this, the calling process
can `fork()` to create a child process. The remaining parent then exits, and the created
child can then safely call `setsid()`.

To avoid this, the calling process may use `setpgid()` to set its own process group ID in
order to create a new process group in the same session in which it is the leader. Then it
can proceed to set the process group ID of all of its children, again with `setpgid()`. This
is allowed, because the caller's process group remains in the same session as the children,
and because it can change the process group ID of its own children. Thus we can do, for example:

```c
pid_t childrenPids*;	/* Say there's a NCHILDREN constant for nubmer of chidren */
/* Create children */

signal(SIGUSR1, SIG_IGN); /* Become immune to SIGUSR1 */

setpgid(0,0); /* Create new process group and become the leader and sole member

pid_t pgrpId; = getpgrp();
for (pid_t *pid = childrenPids; p < childrenPids + NCHILDREN; p++)
	setpgid(*pid, pgrpId);

killpg(pgrpId, SIGUSR1);
```

To eliminate the need to make the process immune to `SIGUSR1`, we could instead select
any child to make a process group leader of their own group, and add the remaining children
to that group.

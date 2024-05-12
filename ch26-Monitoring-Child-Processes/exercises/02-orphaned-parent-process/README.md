# Exercise 26.2

Suppose that we have three processes related as grandparent, parent, child, and that
the grandparent doesn't immediately perform a `wait()` after the parent exits, so that
the parent becomes a zombie. When do you expect the grandchild to be adopted by the
*init* (so that `getppid()`) in the grandchild returns 1): after the parent terminates
or after the grandparent does a `wait()`? Write  program to verify your answer.

## Solution

When the parent exits, the grandparent is still alive. Therefore, the parent becomes
a zombie because the grandparent has not reaped it with `wait()` (or one of its
sibbling functions or system calls). Kerrisk describes in Section 26.2 that when the
parent of a process terminates without doing `wait()`, the *init* process adopts the
child and automatically performs a `wait()`. Based on this, I expect that the grandchild
is adopted after its parent terminates.

I wrote `grandchild_adoption_order.c` and verified this was the case: 

```bash
gcc grandchild_adoption_order.c
./a.out
```

The following is sample output from running the program:

```
Grandparent (31974): Sleeping...
Parent (31975): Creating child...
Parent (31975): Sleeping...
Child (31976): Child is born! Parent is 31975. Sleeping...
Parent (31975): Awake! Exiting.
Child (31976): Child awakes. Parent is now 1853. Sleeping again...
Grandparent (31974): Awake! Reaping child...
Grandparent (31974): Reaped child. Sleeping
Child (31976): Child awakes again. Parent is now 1853. Exiting
Grandparent (31974): Exiting.
```

Before the child goes to sleep, its parent is the process with PID 31975. In the meantime, the parent
exits. When the child first awakes, its parent has PID 1853. In my system, this corresponds to *systemd*.
It's not *init*; see the [TLPI errata](https://man7.org/tlpi/errata/index.html), which goes into
the fact modern init frameworks designated a process whose role is to adopt orphaned children.

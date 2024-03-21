# Exercise 12-02

Write a program that draws a tree showing the hierarchical parent-child
relationships of all processes on the system, going all the way back to
*init*. For each process, the program should display the process ID and
the command being executed. The output of the program should be similar
to that produced by *pstree(1)*, although it does not need to be as
sophisticated. The parent of each process on the system can be found by
inspecting the *PPid:* line of all the `/proc/PID/status` files on the
system. Be careful to handle the possibility that a process's parent
(and thus its `/proc/PID` directory) disappears during the scan of all
`/proc/PID` directories.

## Solution

This one was difficult for me. I was able to open each `/proc/pid/status`
file and read its PID, startup command, and PPID, but the challenge lied in
establishing the links between them. Initially I thought of using a binary
search tree, but of course that was not going to work given that a process
may have more than 2 children processes. However, I used a binary search
tree to store all the processes so that I could search by PID later on.
I realized a graph data structure abstraction would have been ideal to
set up the hierarchy. Instead, I created the following:

```c
struct process {
	pid_t pid;
	pid_t ppid;
	char *cmd;
	struct proclist *firstChild, *lastChild;
};

struct proclist {
	struct process *proc;
	struct proclist *next;
};
```

Here `proclist` represents a linked list containing `process` items.
The `struct process` had the information about each process that I wanted to
display (`pid` and `cmd`), the identifier for its parent process (`ppid`),
a list of children processes, where `firstChild` is the start of the list, and
`lastChild` is the end of the list. Unlike `pstree`, my implementation does
not attempt to keep the processes sorted by name, but it could be achieved
by having changing `proclist` to be a binary search tree ordered by command name.

To get the spacing right, I used the fact that `printf` returns the number of
characters that it output.

I did not do the condensing that `pstree` seems to do. For example, if a lead process
named `postgres` creates 5 follower processes (say, named `postgres-child`),
then my implementation will display them all, as opposed to `pstree` which may display
something along the lines of `5*{postgres-child}`.

### Compiling and Running

Below is sample when compiling and running my implementation:

```c
make
./process_tree
```

```
init(0)->systemd(1)->init-systemd(Ub(2)->init(6)
                                       ->SessionLeader(394)->Relay(396)(395)->bash(396)->process_tree(209754)
                                       ->login(397)->bash(464)
                   ->systemd-journal(45)
                   ->systemd-udevd(68)
                   ->snapfuse(85)
                   ->snapfuse(87)
                   ->snapfuse(88)
                   ->snapfuse(89)
                   ->snapfuse(92)
                   ->snapfuse(95)
                   ->snapfuse(99)
                   ->snapfuse(100)
                   ->systemd-resolve(152)
                   ->cron(184)
                   ->dbus-daemon(186)
                   ->networkd-dispat(190)
                   ->rsyslogd(191)
                   ->snapd(192)
                   ->systemd-logind(193)
                   ->subiquity-serve(221)->python3.10(357)->python3(490)
                   ->unattended-upgr(225)
                   ->agetty(228)
                   ->agetty(231)
                   ->postgres(244)->postgres(248)
                                  ->postgres(249)
                                  ->postgres(267)
                                  ->postgres(268)
                                  ->postgres(269)
                   ->systemd(455)->(sd-pam)(457)
                   ->packagekitd(113584)
                   ->polkitd(113589)
```

Below is the `pstree` command that we were aspiring to (for comparison):
```
systemd─┬─2*[agetty]
        ├─cron
        ├─dbus-daemon
        ├─init-systemd(Ub─┬─SessionLeader───Relay(396)───bash───pstree
        │                 ├─init───{init}
        │                 ├─login───bash
        │                 └─{init-systemd(Ub}
        ├─networkd-dispat
        ├─packagekitd───2*[{packagekitd}]
        ├─polkitd───2*[{polkitd}]
        ├─postgres───5*[postgres]
        ├─rsyslogd───3*[{rsyslogd}]
        ├─snapd───14*[{snapd}]
        ├─8*[snapfuse]
        ├─subiquity-serve───python3.10─┬─python3
        │                              └─5*[{python3.10}]
        ├─systemd───(sd-pam)
        ├─systemd-journal
        ├─systemd-logind
        ├─systemd-resolve
        ├─systemd-udevd
        └─unattended-upgr───{unattended-upgr}
```

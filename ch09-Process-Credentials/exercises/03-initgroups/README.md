# Exercise 09-03

Implement `initgroups()` using `setgroups()` and library functions for retrieving information
from the password and group files (Section 8.4). Remember that a process must be privileged
in order to be able to call `setgroups()`.

## Solution

My solution implements `initgroups()` under the name `my_initgroup()` to avoid clashing
with `initgroups()`, because it includes `<grp.h>` to leverage `setgroups()`, and the
`initgroups()` is declared in that file.

My approach validates that the API client provides a valid user ID and group ID. Then
it builds a list of groups that initially contains only the given group. It then steps
through `/etc/group` by using `getgrent` from `<pwd.h>` and adds to the list any
groups that the user is a member of. It then employs the `setgroups` system call to
attempt to replace the process' supplementary group IDs with the set in the built list.
I tried to be careful of ensuring the `/etc/group` class was closed by calling `endgrent()`
as well as deallocating the memory used to build the group set.

To obtain the maximum possible size of groups, I used `sysconf`, and I resorted to failing
in the scenario where the call failed (possibly because the value is indeterminate).
Admittedly, I could have also counted the number of groups for the given user
by stepping through `/etc/group`.

I made an accompanying main program that tests this. The program expects a command-line
argument with the name of a username in the system whose groups the running process
should be initialized to. The program displays the groups before and after `my_initgroups`.
Note that since the program uses `getgrups()`, it needs to be privileged to run.

```bash
make
sudo ./main $LOGNAME
```

```
Groups before:
0
Groups after:
4
24
27
30
46
100
114
1000
```

# Exercise 09-05

Repeat the previous exercise (09-04) for a process executing a set-user-ID-*root* program,
which has the following initial set of process credentials:

```
real=X effective=0 saved=0
```

### Solution

- Since *Y=0*, the process is privileged, so the behavior of some of the calls changes slightly.
Only the behavior of `setuid` changes (with respect to the previous exercise):

```c
/*
 * When a privileged process calls setuid, the result is that the real, effective, and saved-set
 * user IDs all change to the provided value. Therefore, once we make this call to suspend, we may
 * no longer return to root (unless X=0).
 */

uid_t ruid = getuid();
seteuid(ruid);
seteuid(0);

setreuid(-1, ruid);
setresuid(-1, ruid, -1);
```

- All answers are same as previous exercise, but `setuid` can be used to permanently drop all permissions
in this case with `setuid(getuid());`:

```c
uid_t ruid = getuid();
suid(ruid);
setreuid(ruid, ruid);
setresuid(ruid ruid);
```

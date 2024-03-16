# Exercise 09-02

Exercise Is a process with the following user IDs privileged? Explained your answer.

```
real=0 effective=1000 saved=1000 file-system=1000
```

## Solution

No. A process is privileged when it executes with an effective used ID with a value 0.
However, the an unprivileged process can use the system call `setuid`, `seteuid`, `setreuid`,
or `setresuid` (if available) to change its effective ID to the same value of its real user
ID or its saved set user ID. Thus, it is available to obtain root privilege.


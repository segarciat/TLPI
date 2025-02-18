# Exercise 09-04

If a process whose user IDs all have the value *X* executes a set-user-ID program
whose user ID, *Y*, is nonzero, then the process credentials are set as follows:

```
real=X effective=Y saved=Y
```

(We ignore the file-system user ID, since it tracks the effective user ID.) Show
the `setuid()`, `seteuid()`, and `setreuid()`, calls, respectively, that would
be used to perform the following operations:

- Suspend and resume the set-user-ID identity (i.e., switch the effective user ID
to the value of the real user ID and then back to the saved set-user-ID).
- Permanently drop the set-user-ID identity (i.e., ensure that the effective
user ID and the saved set-user-ID are set to the value of the real user ID).

(This exercise also requires the user of `getuid()` and `geteuid()` to retrieve
the process' real and effective user IDs.) Note that for certain of the system
calls listed above, some of these operations can't be performed.

## Solution

- 

```c
/* Suspend by changing effective user ID to real user ID */
uid_t ssuid = geteuid();
setuid(getuid()); /* Change effective user ID to X */
setuid(ssuid); /* Change effective user ID to Y */

setuid(getuid()); /* Change effective user ID to X */
setuid(ssuid); /* Change effective user ID to Y */

setreuid(-1, getuid()); /* Change effective user ID to X */
setreuid(-1, ssuid); /* Change effective user ID to Y */

setresuid(-1, getuid(), -1); /* Change effective used ID to X */
setresuid(-1, ssuid, -1); /* Changes effective user ID to Y */
```

- For an unprivileged process, `setuid` and `seteuid` only change the
effective user ID. For the `setreuid` call, it is necessary to explicitly
change the saved set userID. If the provided userID is not -1, we ensure
that the saved set user ID gets changed to the same value of the new
effective ID:

```c
/* setuid cannot change the value of the saved set-user-ID */
/* seteuid cannot change the value of the saved set-user-ID */
setreuid(getuid(), getuid());
setresuid(getuid(), getuid(), getuid());
```

# Exercise 15.5

As noted in Section 15.4.6, `umask()` always sets the process umask and, at the
same time, returns a copy of the old umask. How can we obtain a copy of the current
process umask while leaving it unchanged?

## Solution

We can call `umask()` twice; once to get the original value, and another one to
set it back to the original value. The `umask()` call always succeeds:

```c
mode_t originalUmask = umask(0);
umask(originalUmask);
```

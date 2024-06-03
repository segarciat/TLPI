# Exercise 34.3

Write a program to verify that a call to `setsid()` from a process group leader fails.

## Solution

My program begins by calling `setpgrp(0, 0)` to change its own process group ID. The
result is the creation of a new process group whose ID matches that of the calling
process, of which it is the process group leader and the sole member. It then proceeds
to call `setsid()` to attempt to create a new session in which it will become the
leader.

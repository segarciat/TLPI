# Exercise 09-03

Implement `initgroups()` using `setgroups()` and library functions for retrieving information
from the password and group files (Section 8.4). Remember that a process must be privileged
in order to be able to call `setgroups()`.

## Solution

My solution implements `initgroups()` under the name `initgroups_s()` because it includes
`<grp.h>` to leverage `setgroups()`, and the `initgroups()` is declared in that file.
My approach validates that the API client provides a valid user ID and group ID. Then
it builds a list of groups that initially contains only the given group. It then steps
through `/etc/group` by using `getgrent` from `<pwd.h>` and adds to the list any
groups that the user is a member of. It then employs the `setgroups` system call to
attempt to replace the process' supplementary group IDs with the set in the built list.
I tried to be careful of ensuring the `/etc/group` class was closed by calling `endgrent()`
as well as deallocating the memory used to build the group set.

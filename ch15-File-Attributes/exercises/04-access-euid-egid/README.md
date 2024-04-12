# Exercise 15-4

The `access()` system call checks permissions using the process's real user and group IDs.
Write a corresponding function that performs the checks according to the process's effective
user and group IDS.

## Solution

In my implementation, I used `stat()` to get the i-node of the desired file. If it succeeded
and the client only provided `F_OK` in the `mode` flag argument, then I returned 0. I used
`geteuid()` and `getegid()` and returned 0 if either had a value of 0, indicating root user
or root group.

Otherwise, I implemented `eaccess()` based on Permission-Checking Algorithm described in section
15.4.3 on page 297. If the effective user ID matched the file name, I returned 0 if the requested
permissions were granted. Then I checked the supplementary group list (after calling `getgroups()`)
and the effective group. Lastly, if neither of those had the sufficientpermissions, I checked
against "others", finally returning `-1` if none of the checks yielded 0 before.

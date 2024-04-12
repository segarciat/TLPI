# Exercise 15-2

Do you expect any of a file's three timestamps to be changed by the `stat()` system call? If not, explain
why?

## Solution

The three timestamps are: last file access, last file modification, and last status change (change to
the file's i-node information). The `stat()` call returns the i-node information, so the last modification
time should remain intact. A command like `ls` will fail with "*Permission denied*" if it fails to
display the file's metadata because of insufficient permissions. I expect this to be considered a file
access, since we are accessing the file's i-node information, so I, expect the `atime` to change.

However, to verify this, I wrote `stat_timestamps.c`. I found that the value of the last access time
was unchanged each time I ran the program on this README.md file. Therefore, it must be that the
access time refers to the contents, not the metadata. The conclusion is that `stat` does not cause
the file's three timestamps to change.

# Exercise 55-9

Write a pair of programs (or a single program that uses a child process) to bring about
the deadlock situation with mandatory locks described in Section 55.4

## Notes

This section described that mandatory locking can be enabled by doing the following:

1. Enable it on the file system. We can do this by using the `-o mand` option 
with the `mount` command, like so: *mount -o man source target*.

2. Turn on the set-group-ID permission bit and turn off the group execution bit,
by running: *chmod g+s,g-x source/file_to_lock*. Having done this, the group
execution bit on the file will show `S`.

The situation described in Section 55.4 involves two processes where:

1. Two processes open a file in nonblocking mode (i.e., without specifying `O_NONBLOCK`).
2. One process obtains a write lock on a certain byte range of the file.
3. A second process obtains a write lock on a different byte range of the file.
4. The first process attempts to write in the byte range locked by the second file.
5. The second process attempts to write in the byte range locked by the first file.

After the last step, there is a deadlock, which is detected by the kernel and prevented
by the failing write attempt with the error `EDEADLK` for one of the processes.

# Exercise 13-01

Using the *time* built-in command of the shell, try timing the operation of the program
in Listing 4-1 (`copy.c`) on your system.

a) Experiment with different file and buffer sizes. You can set the buffer size using
the `-DBUF_SIZE=nbytes` option when compiling the program.

b) Modify the `open()` system call to include the `O_SYNC` flag. How much difference
does this make to the speed of the various buffer sizes?

c) Try performing these timing tests on a range of file systems (e.g., *ext3*, *XFS*,
*Btrfs*, and *JFS*). Are the results similar? Are the trends the same when going from
small to large buffer sizes?

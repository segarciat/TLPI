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

## Solution

I modified the `makefile` to allow me to pass arguments to the CFLAGS, specifiy,
the `-DBUF_SIZE=nbytes` as advised in the exercises instruction. The line that
allows this with `make` is:

```makefile
override CFLAGS += -I$(TLPI_LIB_PATH)/lib -g -Wall -std=gnu11
```

and the way to set the buffer size via the `make` CLI interface is:

```bash
make CFLAGS="-DBUF_SIZE=2048"
```

a) After an online search, I found that the `fallocate` command can be used to quickly
generate large files, like so:

```bash
# Create binary file named file_mb with 1 MiB length (size)
fallocate -l 1KiB file_mb
fallocate -l 100KiB file_mb
fallocate -l 1MiB file_mb
fallocate -l 1GiB file_mb
```

I created a shell script `a_experiment.sh` that runs tests for file sizes 1MiB through 1024MiB (1GiB).
For each file size (which is passed to `./copy`), it tests for `BUF_SIZE` values from 1 through 65536
(2 to the power of 16). The parameters can be modified in the `a_experiment.sh` through the constants
declared up top. A consistent result is that the system CPU time dominates, with the user time becoming
negligible as the `BUF_SIZE` becomes larger. In my machine, the optimal size seems to be around 16384,
or 2 to the 14th power.

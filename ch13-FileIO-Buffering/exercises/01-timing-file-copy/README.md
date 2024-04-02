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

I modified the `makefile` to allow me to pass arguments to the CFLAGS, specifically,
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
negligible as the `BUF_SIZE` becomes larger. The `results_*` files shown in this directory are from
running it on an `ext4` file system. I found out my computer's file system type by running `df -T .`.
Below is a sample for a file size of `1MiB`, averaged over 5 test runs:

```
1  .9560000000 .0620000000 .8940000000
2  .4740000000 .0140000000 .4600000000
4  .2380000000 .0080000000 .2300000000
8  .1200000000 .0060000000 .1100000000
16  .0600000000 .0040000000 .0540000000
32  .0300000000 .0020000000 .0280000000
64  .0100000000 0 .0100000000
128  0 0 0
256  0 0 0
512  0 0 0
1024  0 0 0
2048  0 0 0
4096  0 0 0
8192  0 0 0
16384  0 0 0
32768  0 0 0
65536  0 0 0
```

Below is a sample of `4MiB`, averaged over 5 test runs:

```
1  3.9260000000 .2520000000 3.6740000000
2  1.9440000000 .1300000000 1.8120000000
4  .9600000000 .0600000000 .8980000000
8  .4800000000 .0400000000 .4380000000
16  .2420000000 .0120000000 .2300000000
32  .1200000000 .0060000000 .1140000000
64  .0600000000 .0040000000 .0560000000
128  .0300000000 .0060000000 .0240000000
256  .0100000000 0 .0100000000
512  .0020000000 0 .0020000000
1024  0 0 0
2048  0 0 0
4096  0 0 0
8192  0 0 0
16384  .0020000000 0 .0020000000
32768  0 0 0
65536  0 0 0
```

b) I created `sync_copy.c` from `copy.c`, modifying the flags for the source and destination files to
include `O_SYNC`. I further added a target to the `makefile` to allow me to build this program, and
modified the shell script to allow me to pass a target of this shell script. The shell script
assumes that the executable created matches the target name and expects two command-line arguments,
which is the case for both `./copy` and `./sync_copy`. Since I anticipated `./sync_copy` to take much
longer, I decided to only do 1 run per size, and in this case I ran for only 1 MiB through 4 MiB.
Sample results are shown for 1MiB:

```
BUF_SIZE    real   user   sys
BUF_SIZE    real   user   sys
1  1933.2700000000 .5200000000 32.4000000000
2  966.6800000000 .3400000000 15.9000000000
4  1060.9600000000 .1700000000 8.0200000000
8  235.5000000000 .0900000000 3.9300000000
16  167.1000000000 .0500000000 2.6300000000
32  59.3200000000 .0200000000 .9700000000
64  68.3100000000 .0200000000 1.0600000000
128  27.9900000000 .0100000000 .4300000000
256  7.4100000000 0 .1200000000
512  3.6800000000 0 .0800000000
1024  1.9200000000 0 .0400000000
2048  .9700000000 0 .0100000000
4096  .5500000000 0 .0200000000
8192  .2200000000 0 0
16384  .2400000000 0 0
32768  .1200000000 0 0
65536  .1000000000 0 0
```

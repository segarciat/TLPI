# Exercise 43.1

Write a program that measures the bandwidth provided by pipes. As command-line arguments, the
program should accept the number of data blocks to be sent and the size of each data block.
After creating a pipe, the program splits into two processes: a child that writes the data blocks
to the pipe as fast as possible, and a parent taht reads the data blocks. After all data has been
read, the parent should print the elapsed time required and the bandwidth (bytes transferred per
second). Measure the bandwidth for different data block sizes.

## Solution

The program I've implemented can be compiled simply with:

```bash
gcc pipe_bandwidth.c
```

The program accepts `-h` or `--help` in its first argument to display a usage message:

```bash
./a.out -h
```

which outputs:

```
Usage: ./a.out [-h | --help] number-of-blocks block-size

Measures pipe bandwidth by forking a child that writes data blocks
which are then read by the parent. Specify block size in bytes
```

In measuring bandwidth, I only measured the time it took the parent to finish read all blocks;
this corresponds to the time necessary to read, using `read()`, the blocks from the pipe (kernel) buffer.
The child uses `write()` to place the data in the pipe buffer.

I ran the program with 1000000 (a million) data blocks, ranging from 512 bytes to 131072 bytes (128 Kibibytes).
It took about 9 seconds of CPU time for 128 Kibibytes, at a rate of about 14769546419.4679 bytes per second.

```
Time elapsed: 0.31358700 secs
Total Bytes Transferred: 512000000
Bandwidth (bytes/sec): 1632720744.1635
```

```
Time elapsed: 0.45303900 secs
Total Bytes Transferred: 1024000000
Bandwidth (bytes/sec): 2260291056.6198
```

```
Time elapsed: 0.66898000 secs
Total Bytes Transferred: 2048000000
Bandwidth (bytes/sec): 3061377021.7346
```

```
Time elapsed: 0.82214600 secs
Total Bytes Transferred: 4096000000
Bandwidth (bytes/sec): 4982083474.2248
```

```
Time elapsed: 1.69839000 secs
Total Bytes Transferred: 8192000000
Bandwidth (bytes/sec): 4823391564.9527
```

```
Time elapsed: 3.62515700 secs
Total Bytes Transferred: 16384000000
Bandwidth (bytes/sec): 4519528395.5978
```

```
Time elapsed: 7.94630300 secs
Total Bytes Transferred: 32768000000
Bandwidth (bytes/sec): 4123678646.5354
```

```
Time elapsed: 11.43229500 secs
Total Bytes Transferred: 65536000000
Bandwidth (bytes/sec): 5732532269.3300
```

```
Time elapsed: 8.87447700 secs
Total Bytes Transferred: 131072000000
Bandwidth (bytes/sec): 14769546419.4679
```

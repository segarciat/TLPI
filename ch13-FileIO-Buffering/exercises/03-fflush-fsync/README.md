# Exercise 13-03

What is the effect of the following statements?

```c
fflush(fp);
fsync(fileno(fp));
```

## Solution

The `fflush()` function flushes the *stdio* buffers, effectively transferring the buffered data
from user space to the kernel buffer cache. The `fileno(fp)` gets the file descriptor associated
with the `FILE` stream `fp`. Calling `fsync()` on the returned file descriptor causes the
kernel buffered data associated with the file descriptor to be flushed to disk, thus forcing
the file to the synchronized I/O file integrity completion state.

In summary, the result of the two calls is that any pending data in stdio buffers is written
to the disk device (or at least its cache), including associated metadata.

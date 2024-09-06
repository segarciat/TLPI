# Exercise 63-5

Modify the program in Listing 63-9 (`self_pipe.c`) to use *poll()* instead of *select()*.

## Solution

In modifying the program, I noticed that there's a possibility that the user may enter
repeated file descriptors (for example, file descriptor `0` is provided twice). Thus
ideally the program would eliminate duplicates given as input, for example, by using
a 'set' data structure. I omitted this for simplicity, and *poll()* reports the status
of all file descriptors on the command-line even if the same value is given multiple
times.

Build as follows:

```bash
make self_pipe_poll
```

Then run, for example, like so:

```bash
# Infinite timeout, only wait for SIGINT
./self_pipe_poll -

# Infinite  timeout, wait for file descriptor 0 and SIGINT
./self_pipe_poll - 0

# 7 second timeout, wait for descriptor 0 and SIGINT
./self_pipe_poll 7 0
```

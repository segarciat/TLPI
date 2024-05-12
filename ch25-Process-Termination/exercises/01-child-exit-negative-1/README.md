# Exercise 25.1

If a child process makes the call `exit(-1)`, what exit status will be seen by
the parent?

## Solution

According to Section 25.1 on page 531, only the bottom 8 bits of the `status` field used
to call `_exit()` are made available to the parent. Since the binary representation of
`-1` has all 1s, this means the parent receives all 1s in the lower 8 bits and 0 for
all the more significant bits. This is equivalent to hex `0xFF`, or decimal 255, which
I hypothesized would be the value seen by the parent

I wrote a small program to verify this by calling `fork()` and then using the `wait()`
system call on the parent to reap the child and obtain the exit status. I noticed that
the hex value was `0xff00`, or decimal 65280, though I expected 255 because I thought
the value would be the least significant byte of `-1`, which is `0xff`.

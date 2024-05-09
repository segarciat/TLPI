# Exercise 25.1

If a child process makes the call `exit(-1)`, what exit status will be seen by
the parent?

## Solution

According to Section 25.1 on page 531, only the bottom 8 bits of the `status` field used
to call `_exit()` are made available to the parent. Since the binary representation of
`-1` has all 1s, this means the parent receives all 1s in the lower 8 bits and 0 for
all the more significant bits. This is equivalent to hex `0xFF`, or decimal 255.

I wrote a small program to verify this but noticed that the hex value was `0xff00`.
After some thought, I realized that its because my machine uses little-endian ordering,
so the least significant byte appears at the earlier addresses.

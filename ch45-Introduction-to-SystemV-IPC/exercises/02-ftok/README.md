# Exercise 45.2

Implement `ftok()`.

## Solution

Based on Exercise 45.1, I concluded that the 2 least significant bytes of the file's i-node
are used as the 2 least significant bytes of the IPC key, the next most significant byte
is the minor device number, and finally, the most significant byte is the least significant
byte of the `proj` argument.

The program I created expects a filename, and it randomly computes a `proj` value between 1
and 255 to use in computing the IPC key using both the real `ftok()` and my implementation
named `ftok()`:

```bash
gcc main.c ftok.c
./a.out
```

Sample output:

```
Generated proj value: 203
ftok() : -888860831
_ftok(): -888860831
```

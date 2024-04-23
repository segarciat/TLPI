# Exercise 18.3

Implement `realpath()`.

## Solution

I named my implementation `s_realpath` to avoid clashing with the existing `realpath`
in `<stdlib.h>`. In addition to the implementation, I've also added a client program for testing:

```bash
gcc -o my_realpath main.c s_realpath.c
./my_realpath /home/../etc/passwd
# output: /etc/passwd
```

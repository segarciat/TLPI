# Exercise 05-07

Implement `readv()` and `write()` using `read()`, `write()`, and suitable fnuctions from the `malloc` package.

## Solution

The example `t_readv.c` was used to test the `readv` implementation. I also implemented `writev`, but
skipped the testing.

```bash
make
cp README.md tfile
./t_readv tfile

make clean
rm tfile
```

Expected output:

```
total bytes requested: 248; bytes read: 248
```

# Exercise 04-02

Write a program like `cp` that, when used to copy a regular file that contains
holes (sequences of null bytes), also creates corresponding holes in the target
file.

## Solution

My implementation makes use of the heuristic that any sequence of null bytes
indicates a hole. Of course this may not necessarily be the case, since
a null byte could have been written explicitly to the file.

```bash
# Build executable

# Copy README
./copy README.md README-COPY.md
diff README.md README-COPY.md
```

The `diff` command does not show output because the files are identical.
Next, we use the `seek_io` program from Listing 4.3 to create a file
with holes and verify whether the holes are preserved after copying.

```bash
# Create file with 4 bytes and a hole
touch tfile
../../../tlpi-dist/fileio/seek_io tfile s100000 wabc
./copy tfile tfile-copy
```

Running `ls -l tfile tfile-copy` reports:

```
-rw-rw-r-- 1 sgarciat sgarciat 100003 Feb 12 16:38 tfile
-rw-r--r-- 1 sgarciat sgarciat 100003 Feb 12 16:39 tfile-copy
```

Running `du -k tfile tfile-copy` reports:

```
4	tfile
4	tfile-copy
```

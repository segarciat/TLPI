# Exercise 05-02

Write a program that opens an existing file for writing with the `O_APPEND` flag, and
then seeks to the beginning of the file before writing some data. Where does the data
appear in the file? Why?

## Solution

Based on the implementation in this directory, I found that the content is written
to the end of the file, and not the beginning. This is because the `O_APPEND` access
mode ensures that writes to the file happen at the end. The call atomically seeks
to end and writes the content. This renders any previous `lseek` redundant.

First make a test file:

```bash
# -e tells it to enable interpreting baskslash escape sequences
echo -e "apple\nblueberries\ncherries" > tfile
cat tfile
```

The contents will be:

```
apple
blueberries
cherries
```

Then build and run:

```bash
make
# -e tells it to enable interpreting baskslash escape sequences
echo -e "apple\nblueberries\ncherries" > tfile
./append_seek tfile
```

The expected output is:

```
After opening file, the file offset is at: 0
After seeking to start, the file offset is at: 0
After writing file, the file offset is at: 43
```

The file's contents will be:

```
apple
blueberries
cherries

first
second
```

Clean up:

```bash
make clean
rm tfile
```

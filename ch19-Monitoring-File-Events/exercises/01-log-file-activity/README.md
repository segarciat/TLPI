# Exercise 19.1

Write a program that logs all file creations, deletions, and renames under the
directory named in its command-line argument. The program should monitor events
in all of the subdirectories under the specified directory. To obtain a list of
all these subdirectories, you will need to make use of `nftw()` (Section 18.9).
When a new subdirectory is added under the tree or a directory is deleted, the
set of monitored subdirectories should be updated accordingly.

## Solution

I used a global inotify ID and also a global linked list to keep track of the list
of watched directories.

```bash
make
./log_file_events . &

# when done
fg
# Press CTRL-D
```

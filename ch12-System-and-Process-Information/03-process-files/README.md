# Exercise 12-03

Write a program that lists all processes taht have a particular file pathname open.
This can be achieved by inspecting the contents of all the `/proc/PID/fd/*` symbolic
links. This will require nested loops employing *readdir(3)* to scan all `/proc/PID`
directories, and then the contents of all `/proc/PID/fd` entries within each `/proc/PID`
directory. To read the contents of a `/proc/PID/fd/n` symbolic link requires the use of
*readlink()*, described in Section 18.5.

## Solution

My solution expects exactly 1 command-line argument, representing the path of a file.
Then it goes on to file processes that have this file opens. I used `realpath` from
`stdlib.h` to canonicalize the given path (see `man 3 realpath`). Since a process
may have multiple file descriptors for a given file, I enwsured that the process
number was only displayed once.

### Compile and Run

```bash
make
# Requires process to be privileged
sudo ./process_using_file /dev/null
```

I picked `/dev/null` as a sample file to look for, and the output was as follows:

```
1
2
6
45
68
85
87
88
89
92
95
99
100
152
184
186
190
191
192
193
221
225
244
248
249
267
268
269
357
455
457
490
113584
113589
210655
210656
```

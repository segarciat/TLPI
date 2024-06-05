# Exercise 35.2

Write a set-user-ID-*root* program that is the realtime scheduling analog of *nice(1)*.
The command-line interface of this program should be as follows:

```
# ./rtsched policy priority command arg...
```

In the above command, *policy* is *r* for `SCHED_RR` or *f* for `SCHED_FIFO`. THis program should drop
its privileged ID before execing the command, for the reasons described in Section 9.7.1 and 38.3

## Solution

```bash
# Compile
gcc -o rtsched rtsched.c

# Make root the owner
sudo chown root ./rtsched
sudo chgrp root ./rtsched

# Make into a set-user-ID-root program
sudo chmod u+s ./rtsched

# Run as regular user
./a.out r 1 ls
```

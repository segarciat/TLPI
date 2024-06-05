# Exercise 36.2

Write a program that executes a command and then displays its resource usage. This is analogous
to what the *time(1)* command does. Thus, we would use this program as follows:

```
$ ./rusage command arg...
```

## Solution

I decided to only display the user and system CPU time because of the weak specification by
SUSv3 and the fact that other fields are non-standards.

```bash
# Compile
gcc rusage.c

# Run a command
./rusage find . -type f
```

The output is as follows:

```
./rusage.c
./README.md
./rusage

User CPU time: 0.003286s
System CPU time: 0.000000s
```

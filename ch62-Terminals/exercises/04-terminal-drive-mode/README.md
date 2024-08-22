# Exercise 62-4

Write a program that displays information indicating whether the terminal referred
to by standard input is standard input is in canonincal or noncanonical mode. If
in noncanonical mode, thendisplay the values of TIME and MIN.

## Solution

This one was easy. Just had to check whether the `ICANON` *local mode* flag was set
(the *c_lflag* field in the *termios* structure):

```bash
gcc -Wall -o tmode tmode.c
./tmode
```

For me, the output was:

```
Canonical
```

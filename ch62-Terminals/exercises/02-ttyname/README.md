# Exercise 62-2

Implement *ttyname()*.

## Solution

According to Section 62-10, *ttyname()* uses the *opendir()* and *readdir()* functions
to walk through the directories holding terminal device  names, looking at each directory
entry until it finds one whose device ID (`st_rdev`) field matches that of the device
referred to by file descriptor `fd`. Moreover, it mentions that terminal entries normally
reside in `/dev` with entries such as `/dev/tty*` or `/dev/pts/*`.

My implementation does just this. I created a `tty.c` program that, similar to the *tty(1)*
command, displays the terminal associated with standard input:

```bash
gcc -Wall -o tty tty.c ttyname.c
./tty
```

On my system, I get:

```
/dev/pts/1
```

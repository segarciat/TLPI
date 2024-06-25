# Exercise 40.1

Implement `getlogin()`. As noted in Section 40.5, *getlogin()* may not work correctly for
processes running under some software terminal emulators; in that case, test from a virtual
console instead.

## Solution

As described in Section 40.5, the *getlogin()* function returns the name of the user
logged in on the controlling terminal of the calling process. It uses information
in the `utmp` file:

```c
#include <unistd.h>

char *getlogin(void);
```

*getlogin()* calls *ttyname()* to find the name of the terminal associated with the
calling process's standard input. Then, it searches the `utmp` file for a record
whose *ut_line* matches the terminal name. If a matching record is found, then *getlogin()*
returns the *ut_user* string from that record. If a match is not found or an error occurs,
then *getlogin()* returns `NULL` and sets `errno` to indicate the error.

I attempted to implement it under the name `getlogin_s()` to avoid clashing with the glibc implementation.
However, I was unsuccessful, as you can see from the following session:

```bash
gcc main.c getlogin.c
./a.out
```

which had output

```
./a.out: Failed to get login name (No such file or directory)
./a.out: However, glibc getlogin() was successful: sgarciat
```

After investingating a bit, I ran *who(1)* and *tty(1)* and got the following output:

```bash
tty
# /dev/pts/0
who -H
# NAME     LINE         TIME             COMMENT
# sgarciat :1           2024-06-25 09:33 (:1)
```

The LINE column should have `/dev/pts/0`, which is the terminal name associated with standard input,
but instead it has `:1`. I was unable to find out what `:1` stands for, so I will forego this exercise
for now.


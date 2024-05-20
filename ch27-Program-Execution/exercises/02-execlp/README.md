# Exercise 27.2

Use `execve()` to implement `execlp()`. You will need to use the *stdarg(3)* API to handle the
variable-length argument list supplied to `execlp()`. You will also need to use functions in
the *malloc* package to allocate space for the argument and the environment vectors. Finally,
note that an easy way of checking whether a file exists in a particular directory and is
executable is simply to try execing the file.

## Solution

I gathered the following requirements for the implementation of `execlp()` from chapter 27:

- The `l` indicates that the caller passes the arguments intended for the underlying `execve()`
call as a *list* of `char *` strings, terminated by a `(char *) NULL`, instead of a *vector*
of type `char *[]`. Hence, this is why we need to use `stdarg.h` to handle variable-length arguments.

- The `p` indicates that the first argument may be interpreted as a filename or as a path.
If the string in the first argument *contains* (not begins with) a `/`, then it is treated as
an absolute path or relative path. In this case, `execve()` is called with this value. Otherwise,
it is treated as a filename, and the directories specified by the `PATH` environment variable are
searched in order for the filename. (see 27.2, page 567). When `PATH` is not defined, it assumes
a default path of `.:/usr/bin:/bin`.

- When `execlp()` searches for the file to execute in the directories specified in `PATH`, it
continues until a file with a given name is successfully execed (see 27.2.1, page 569) meaning
that if it is found early in the list but fails (perhaps due to not being executable) it continues
the search in other directories.

- When using `execve()`, remember that conventionally the first argument is the either an absolute
pathname to the executed program, or just the basename. Also ensure that it is `NULL`-terminated
(see page 27.2 on page 568).

- The environment is inherited from the calling process. This can be attained by using passing in
the global `environ` variable, declared at the top-level as `extern char **environ` or
`extern char *environ[]`.

- Be sure to keep track of `errno` throughout calls in `execlp()` fails, perhaps because there was
no memory to allocate for the argument list provided, or because `execve()` failed, etc. This
may mean employing a method similar to the signal handlers, where we save `errno` before doing
certain system calls, and restore it after, if there's a failure.

- The specified file needs to be an executable file in a format such as ELF, or an executable script.
Page 565 specifies that `execve()` fails with `ENOEXEC` if the file is executable but does not have
a leading `#!` line specifying the path of the shell. Section 27.3 on page 575 specifies that
if `execlp()` finds an executable file that is not a binary executable nor does it start with a `#!`,
then they exec the interpreter to interpret the file. Moreover, in Linux it is as if the file had
`#!/bin/sh`. Therefore, when implementing the file, check for `ENOEXEC` failures, and retry by
using `execve()` using `/bin/sh` as the program path, and modify the arguments accordingly.
According to the man page (see `man execlp`), "If  this  attempt fails, no further searching is done."

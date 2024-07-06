# Exercise 44.2

Implement `popen()` and `pclose()`. Although these functions are simplified by not requiring
the signal handling employed in the implementations of `system()` (Section 27.7), you will
need to be careful to correctly bind the pipe ends to file streams in each process, and to
ensure that all unused descriptors referring to the pipe ends are closed. Since children
created by ymultiple calls to `popen()` may be running at one time, you will need to maintain
a data structure that associates the file stream pointers allocated by `popen()` with the
corresponding child process IDs. (If using an array for this purpose, the value returned by
`fileno()` function, which obtains the file descriptor corresponding to a file stream, can
be used to index the array.) Obtaining the correct process ID from this structure will allow
`pclose()` to select the child upon which to wait. This structure will also assist with the
SUSv3 requirement that any still open file streams created by earlier calls to `popen()`
must be closed in the new child process.

## Solution

The `popen()` function does the following:

- It execs a shell to execute the string given in its *command* argument.
- It accepts `"r"` (calling process reads from pipe) or `"w"` (calling process wrirtes to pipe 
in its second argument (returns `NULL` if neither).
- Returns file stream pointer usable with *stdio*.


The `pclose()` function does the following:

- When I/O is complete, `pclose()` closes the pipe and waits for the child shell to terminate.
It return the termination status of the last commandt at the shell executed, unless it was
terminatd by a signal.
- If a shell could not be execed, `pclose()` returns as though the child shell terminated with `_exit(127)`.
- On other error, `pclose()` returns `-1`.
- While waiting to obtain the status of the child shell, `pclose()` must ensure that the
internal call to `waitpid()` is restarted if interrupted.
- Unlike `system()`, `popen()` should *not* ignore `SIGINT` and `SIGQUIT`.
- Should not block `SIGCHLD`.

I implemented the commands and tried to stay true to these specifications. I also wrote a
program `main.c` that tests with `"r"` and `"w"` mode. It does so using the `ls` command and
the `cat` command:

```bash
gcc main.c popen_pclose.c
./a.out
```

The output is:

```
a.out
main.c
popen_pclose.c
popen_pclose.h
README.md
Child should print this out
```

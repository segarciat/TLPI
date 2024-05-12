# Exercise 26.3

Replace the use of `waitpid()` with `waitid()` in Listing 26.3 (`child_status.c`). The
call to `printWaitStatus()` will need to be replaced by code that prints relevant fields
from the `siginfo_t` structure returned by `waitid()`.

## Solution

First I replaced the `waitpid()` call with `waitid()`:

```c
siginfo_t info;
if (waitid(P_ALL, 0, &info, WEXITED | WSTOPPED
#ifdef WCONTINUED
	| WCONTINUED
#endif
	) == -1)
	errExit("Wait");
```

I specified `P_ALL` to wait for any child, which means the following ID argument is ignored
(here I just specified it as 0). I passed a pointer to the `info` variable of type `siginfo_t`.
Finally, I specified `WEXITED | WSTOPPED | WCONTINUED` in the last argument. This ensures
that if a child is terminated normally or abnormally (`WEXITED`), is stopped by a signal (`WSTOPPED`),
or is resumed by `SIGCONT` (`WCONTINUED`) then the parent is informed and `waitid()` returns.
Unlike `waitpid()`, the `waitid()` system call returns 0 on success, not the PID of the child.
Like in the original implementation, `WCONTINUED` iw wrapped with `#ifdef` because it is not
present on older Linux versions.

Instead of `printWaitStatus()`, I defined an equivalent `printSiginfoWaitStatus()` that
displays almost the same information, but using the `siginfo_t` structure. I did not use the
macros `WEXITSTATUS()`, `WTERMSIG()`, or `WSTOPSIG()` because they apply to a `status` returned
by `wait()` or `waitpid()`, as described on page 546 (Section 26.1.3).

Build with `make` and run:

```bash
make
./child_status
# Child started with PID = 33716
```

I used the `kill` command to send signals to it:

```bash
kill -s SIGSTOP 33716
kill -s SIGCONT 33716
kill 33716 # Send SIGTERM
```

The output was as follows:

```
Child started with PID = 33716
waitid() returned: PID=33716; status=0x0013 (0,19)
child stopped by signal 19
waitid() returned: PID=33716; status=0x0012 (0,18)
child continued
waitid() returned: PID=33716; status=0x000f (0,15)
child killed by signal 15
```

# Exercise 18.9

In Section 18.10, we showed two different techniques (using `fchdir()` and `chdir()`, respetively)
to return to the previous current working directory after changing the current working directory to
another location. Suppose we are performing such an operation repeatedly. Which method do you expect
to be more efficient? Why? Write a program to confirm your answer.

## Solution

The first approach used the `open()` system call:

```c
int fd;

fd = open(".", O_RDONLY);	/* Remember where we are */
chdir(somepath);			/* Go somewhere else */
fchdir(fd);					/* Return to the original directory */
close(fd);
```

The second approach used `getcwd()`:

```c
char buf[PATH_MAX];

getcwd(buf, PATH_MAX);	/* Remember where we are */
chdir(somepath);		/* Go somewhere else */
chdir(buf);				/* Return to original directory */
```


Which approach is more efficient comes down to which of `fchdir()` or `chdir()`
is more efficient, sasuming that `getcwd()` and `open()` are each only called once
for the sake of remembering the old working directory.

I suspect that the second approach is slower because it probably needs to validate the
path and ensure the process has appropriate permissions.

I wrote `test.c` which changes directories 1,000,000 times to test this, and consistently
the `fchrdir()` and `open()` approach was faster:

```bash
fchdir() and open(): 0.89
chdir() and getcwd(): 1.97
```

The book's Appendix agreed that `fchdir()` should be faster, but the reason given is that
more data needs to be transferred from user space to kernel space via the call to `chdir()`.

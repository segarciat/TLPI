# Exercise 18.4

Modify the program in Listing 18-2 (`list_files.c`) to use `readdir_r()` instead of `readdir()`.

## Solution

I allocated space for a `struct dirent` large enough to hold any path in accordance to the
advice on Section 18.8 as follows:

```c
struct dirent *dp;
size_t len;
len = offsetof(struct dirent, d_name) + NAME_MAX + 1;
if (dp == NULL)
	errExit("malloc");
```

I included `<limits.h>` for the `NAME_MAX` constant, `<stddef.h>` for the `offsetof()` macro,
and `<stdlib.h>` for `malloc()`.

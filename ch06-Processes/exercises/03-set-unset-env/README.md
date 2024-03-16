# Exercise 06-03

Implement `setenv()` and `unsetenv()` using `getenv()`, `putenv()`, and, where necessary,
cod ethat directly modifies `environ`. Your version of `unsetenv()` should check to see
whether there are multiple definitions of an environment variable, and remove thme all
(which is what the *glibc* version of `unsetenv()` does).

## Solution

Since the exercise called for using `putenv()` and `getenv()` to implement `setenv()` and
`unsetenv()`, I used `<stdlib.h>`. To avoid a name clash, I appended `_s` to each, so I
instead called them `setenv_s()` and `unsetenv_s()`.

In my solution, I was careful to look out receiving `NULL` for either `name` or `value`,
watched out for overflow in the length of the combined string of form *name=value* pair
provided to `setenv_s()`. For `unsetenv_s()`, I was wanted to ensure I omitted `=` in
the comparison for the environment variable to remove, so I used `strncmp` instead of `strcmp`,
though first I checked that `=` wasn't a in the provided environment variable name.

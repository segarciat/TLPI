# Exercise 06-03

Implement `setenv()` and `unsetenv()` using `getenv()`, `putenv()`, and, where necessary,
code that directly modifies `environ`. Your version of `unsetenv()` should check to see
whether there are multiple definitions of an environment variable, and remove them all
(which is what the *glibc* version of `unsetenv()` does).

## Solution

Since the exercise called for using `putenv()` and `getenv()` to implement `setenv()` and
`unsetenv()`, I used `<stdlib.h>`, defining the feature macro `_DEFAULT_SOURCE` to expose
necessary definitions, such as that of `putenv()`. To avoid a name clash, I prepended `my_`
to each, so I instead called them `my_setenv()` and `my_unsetenv()`.

In my solution, I was careful to look out receiving `NULL` for either `name` or `value`,
and watched out for overflow in the length of the combined string of form *name=value* pair
provided to `my_setenv()`. 

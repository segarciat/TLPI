# Exercise 08-01

When we execute the following code, which attempts to display the usernames for two different user IDs,
we find that it displays the same username twice. Why is this?

```c
printf("%s %s\n", getpwuid(uid1)->pw_name, getpwuid(uid2)->pw_name);
```

## Solution

As described in section 8.4, `getpwuid` returns a pointer to a statically allocated structure
that is overwritten on each call to `getpwuid`. Both calls are evaluated before they
are passed to `printf`. Regardless of which call to `getpwuid` is evaluated last,
the result is that both will point to the same statically allocated buffer containing
the name.

# Exercise 08-02

Implement `getpwnam()` using `setpwent()`, `getpwent()`, and `endpwent()`.

## Solution

The implementation is straightforward. The `main.c` mimics the chapter listing,
requesting a username string from the user, and displaying their UID if a match
is found, exiting with an error message in the event of no match or failure.

```bash
# Compile
gcc main.c getpwnam_s.c

# Run
./a.out
```

The following is sample output when providing `root` as the username:

```
Username: root
Found: root with UID 0
```

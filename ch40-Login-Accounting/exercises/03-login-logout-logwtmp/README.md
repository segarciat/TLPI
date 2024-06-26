# Exercise 40.3

Read the manual pages for *login(3)*, *logout(3)*, and *logwtmp(3)*. Implement these functions.

## Solution

After implementing the function, I was able to verify with the *who(1)* command that the `utmp` entry
was modified correctly, but the *last(1)* command did not show in its output the intended change.
Moreover, I realized after after some checks for `errno` and doing print-debugging that `setutent()`
was failing with "No such file", and same with `updwtmp(__PATH_WTMP, &ut)`. I was, however, unable
to figure out why. I will forego this exercise.

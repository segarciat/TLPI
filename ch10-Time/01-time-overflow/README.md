# Exercise 10-01

Assume a system where the value returned by the call `sysconf(_SC_CLK_TCK)` is 100.
Assuming that the `clock_t` returned by `times()` is a signed 32-bit integer, how
long will it take before this value cycles so that it restarts at 0? Perform the
same calculation for the `CLOCKS_PER_SEC` returned by `clock()`.

## Solution

A 32-bit `clock_t` has a maximum value of 2^{31}-1, or `2147483647`. The interpretation
of `clock_t` is different depending on whether it was obtained by calling `times()` or
`clock()`.

When calling `times()`, the `clock_t` data type measures time in units of *clock ticks*,
and the value of `sysconf(_SC_CLK_TCK)` gives the number of clocks per second. We can
divide a `clock_t` value by this number to convert to seconds. Dividing the maximum value
of `clock_t` by 100 yields `21474836.47` seconds. Since a day has `86400` seconds (60 seconds in
a minute, 60 minutes in an hour, 24 hours in a day), further dividing by `86400` yields
approximately 248 days. At this point, the value will overflow `clock_t` and it cycles
back to 0.

For `clock_t` as returned by `clock()`, the value is measured in units of `CLOCKS_PER_SEC`,
by which we must divide to arrive at the nubmer of seconds in CPU time used by the process.
It is fixed at 1 million by POSIX.1. When we do so, we get approximately 2147 seconds, which
is approximately 36.79 minutes, or 36 minutes and 47 seconds.

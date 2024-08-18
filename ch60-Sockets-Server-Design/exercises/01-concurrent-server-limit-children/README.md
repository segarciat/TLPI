# Exercise 60-1

Add code to the program in Listing 60-4 (`is_echo.sv.c`) to place a limit on the
number of simultaneously executing children.

## Solution

I defined a constant `MAX_CHILDREN` to limit the number of children created.
I also created a global variable `numChildren` that is initially set to `0`.
This variable keeps track of the number of children alive. I made the following
design decisions:

1. Each time the signal handler reaps a child process, it decrements `numChildren`.

2. I increment the value of `numChildren` prior to the call to *fork()* and
decrement it if *fork()* fails. This way, in the scenario that the child exits
before the parent is scheduled, the count does not become negative.

3. I typed the `numChildren` global variable a (signed) `long`. This way, in
the scenario that the count falls into the negative range, perhaps because
of the race between *main()* and the `SIGCHLD` handler, the *main()* function
is not blocked. This could happen if, say, an unsigned type such as `size_t`
were used, in which case if `numChildren` were `0` and we decremented it by
`1`, then it would wrap around to the maximum positive value. In this case,
the comparison `numChildren >= MAX_CHILDREN` would erroneously succeed.

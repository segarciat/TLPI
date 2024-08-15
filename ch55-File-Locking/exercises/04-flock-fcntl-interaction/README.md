# Exercise 55-4

Experiment by running the programs in Listing 55-1 (`t_flock.c`) and Listing 55-2
(`i_fcntl_locking.c`) to see whether locks granted by *flock()* and *fcntl()* have
any effect on one another. If you have access to other UNIX implementations,  try the
same experiment on those implementations.

## Solution

I tried `t_flock` to acquire an exclusive lock for 1 minute while interactively attempting
to lock the file with `i_fcntl_locking`. I found that `i_fcntl_locking` always succeeded
in obtaining both read and write locks. I then changed to acquiring a shared *flock()*
for 1 minute with `t_flock`, and observed the same behavior for `i_fcntl_locking`.
Then I switched it, interactively locking `README.md` before locking with `t_flock`,
and I observed that *flock()* was successful in this case, too.

I concluded that the two locks do not have any effects on one another.

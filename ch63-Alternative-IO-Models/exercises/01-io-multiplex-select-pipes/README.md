# Exercise 63-1

Modify the program in Listing 63-2 (`poll_pipes.c`) to use *select()* instead  of *poll()*.

## Solution

To accomplish the conversion, I replaced the `struct poll` list with an `fd_set` object
holding the *read set*. Since the maximum size is dictated by the `FD_SETSIZE` constant,
I also added a check that the number of pipes requested does not exceed this value.

Finally, after the *select()* returns, the remaining task is to use `FD_ISSET` to verify
which descriptors are set in the read set.

```bash
make
./select_pipes 32 2
```

Sample output:

```
Writing to fd:  22 (read fd:  21)
Writing to fd:   6 (read fd:   5)
select() returned: 2
Readable:   5
Readable:  21
```

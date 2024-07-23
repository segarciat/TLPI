# Exercise 48-4

Write a program that displays the contents of the *shmid_ds* structure (Section 48.8)
associated with a shared memory segment. The identifier of the segment should be specified
as a command-line argument. (See the program in Listing 47-3, on page 973, which performs
the analogous task for System V semaphores.)

## Solution

Compile and run:

```bash
gcc svshm_info.c
./a.out --help
./a.out 7
# Compare with ipcs
ipcs -m
```

Sample output:

```

Created by PID 2882 at Sun Jul 21 15:00:17 2024
Last attached at Sun Jul 21 15:00:21 2024
Last dettached at Sun Jul 21 15:00:21 2024

id           owner      perms        size            nattach
7            sgarciat   600         524288          2
```

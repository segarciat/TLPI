# Exercise 48-6

Write a program (analogous to program in Listing 46-6, on page 953) that uses the
*shmctl()* `SHM_INFO` and `SHM_STAT` operations to obtain and display a list of all
memory segments on the system.

## Solution

```bash
gcc svshm_ls.c
./a.out
```

Sample output:

```
Maximum Index: 62
index    id           owner      perms        size            nattach
4        4            sgarciat   600         524288          2
7        7            sgarciat   600         524288          2
24       24           sgarciat   600         524288          2
51       51           sgarciat   600         524288          2
52       52           sgarciat   600         524288          2
62       62           sgarciat   600         524288          2
```

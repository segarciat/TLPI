# Exercise 01

Modify the program in Listing 7-1 (`free_and_sbrk.c`) to print out the current value
of the program break after each execution of `malloc()`. Run the program, specifying
a small allocation block size. This will demonstrate that `malloc()` doesn't employ
`sbrk()` to adjust the program break on each call, but instead periodically allocates
larger chunks of memory from which it passes back small pieces to the caller.

## Solution

To build the executable, run

```bash
make
```

I repurposed the `freeMin`, `freeMax`, and `freeStep` variables, renaming them to
`mallocMin`, `mallocMax`, and `mallocStep`. Then I made the following call:

```bash
./free_and_sbrk 10000 8 1000
```

This tells the problem to allocate 10000 blocks, each of size 8, and then
to free them in steps of 1000. However, instead of freeing the blocks,
the program calls `sbrk()` to report the address of the program break.
The result was

```

Initial program break:          0x55d379f77000
Allocating 10000*8 bytes
After call to malloc, program break is now: 0x55d379f77000
After call to malloc, program break is now: 0x55d379f77000
After call to malloc, program break is now: 0x55d379f77000
After call to malloc, program break is now: 0x55d379f77000
After call to malloc, program break is now: 0x55d379f77000
After call to malloc, program break is now: 0x55d379f98000
After call to malloc, program break is now: 0x55d379f98000
After call to malloc, program break is now: 0x55d379f98000
After call to malloc, program break is now: 0x55d379f98000
After call to malloc, program break is now: 0x55d379fb9000
```

As you can see, the initial program break was `0x55d379f77000`. When allocating 1000 blocks,
each of size 8 bytes, it turns out that `malloc` did not increase the program break after
5 calls. By then, `malloc` had already allocated `5000*8` bytes. On the 6th call, the program
break was finally increased to `0x55d379f98000`. It then remained unchanged until the 10th
call, when it increased to `0x55d379fb9000`. It's evident, as advertised by this exercise, that
`malloc` does not call `sbrk` in every call.

We can also use the Linux-specific `strace` to analyze this:

```bash
strace -e trace=brk ./free_and_sbrk 10000 8 1000
```

```
00 8 1000
brk(NULL)                               = 0x5e7bd3a7e000
brk(NULL)                               = 0x5e7bd3a7e000
brk(0x5e7bd3a9f000)                     = 0x5e7bd3a9f000

Initial program break:          0x5e7bd3a9f000
Allocating 10000*8 bytes
After call to malloc, program break is now: 0x5e7bd3a9f000
After call to malloc, program break is now: 0x5e7bd3a9f000
After call to malloc, program break is now: 0x5e7bd3a9f000
After call to malloc, program break is now: 0x5e7bd3a9f000
After call to malloc, program break is now: 0x5e7bd3a9f000
brk(0x5e7bd3ac0000)                     = 0x5e7bd3ac0000
After call to malloc, program break is now: 0x5e7bd3ac0000
After call to malloc, program break is now: 0x5e7bd3ac0000
After call to malloc, program break is now: 0x5e7bd3ac0000
After call to malloc, program break is now: 0x5e7bd3ac0000
brk(0x5e7bd3ae1000)                     = 0x5e7bd3ae1000
After call to malloc, program break is now: 0x5e7bd3ae1000
+++ exited with 0 +++
```

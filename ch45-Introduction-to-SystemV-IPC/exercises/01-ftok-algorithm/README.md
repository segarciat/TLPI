# Exercise 45.1

Write a program to verify that the algorithm employed by *ftok()* uses the
file's i-node number, minor device number, and *proj* value, as described in
Section 45.2. (It is sufficient to print all of these values, as well as the
return value from *ftok()*, in hexadecimal, and inspect the results for a few
examples.)

## Solution

My program uses `srand()` and `rand()` to generate a random integer for the `proj`
argument of `ftok()` that falls in the interval `1` through `255` by using the
modulus operator. It also uses the `mkstmp()` function to generate a temporary
file whose path is passed to `ftok()`, and is later deleted with the `unlink()`
system call. To compile and run, issue the following:

```bash
gcc ftok_algorithm.c
./a.out
```

The following is a sample output of the program:

```
Generated proj (2 LSBs): 25
Successfully created temporarily file with name: ./tmpftok2nTqZB
Successfully created System V IPC key: 419762002

Filename           : ./tmpftok2nTqZB
Minor Device Number: 05
i-node (2 LSBs)    : 0f52
proj (LSB)         : 19
key (LSB)          : 19050f52
Deleting the temporary file
```

The `key_t` data type is a 32-bit integer. Notice that the two least significant bytes of the
i-node form the 2 least significant bytes of the key. The next most significant byte is
the device number. Finally, the *proj* argument is placed in the next most siginificant byte.

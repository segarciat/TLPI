# Exercise 13-04

Explain why the output of the following code differs depending on whether standard output is
redirected to a terminal or to a disk file.

```c
printf("If I had more time, \n");
write(STDOUT_FILENO, "I would have written you a shorter letter.\n", 43);
```

## Solution

In both cases, the `write` call places the content in the kernel buffer cache.


The buffering behavior employed by the *stdio* library changes depending on the stream.
In the case of a terminal, the default is `_IOLBF`, which means line-buffered I/O.
This means that data is buffered until a new line character is output. Meanwhile,
for a disk file, `_IOFBF` is employed, which means fully-buffered I/O. In this case,
data is written in units equal to the size of the buffer.

Since the size of the *stdio* buffer is likely much larger than the string `If I had more time, \n"`,
this means that in the case of a disk file, the string will not yet be in the kernel buffer cache.
Meanwhile, for a terminal device, it will be.

Hence, the likely result for a terminal device will be:

```
If I had more time, 
I would have written you a shorter letter.

```

In the case of a disk file, it will liely be:

```
I would have written you a shorter letter.
If I had more time, 

```

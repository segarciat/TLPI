# Exercise 49-4

Write a program that uses the `MAP_FIXED` technique described in Section 49.10 to create
a nonlinear mapping similar to that shown in Figure 49-5.

## Solution

As described in Section 49.10, Calling *mmap()* with `MAP_FIXED` and specifying an *addr*
and *length* whose corresponding region overlaps another mapped region has teh effect of
replacing the old mapping with the new one specified. The portable way to achieve this,
as described in the text, is to first create an anonymous mapping without specifying
`MAP_FIXED`, to allow the kernel to choose a valid address. Then, we use *mmap()* to
map (overlay) file regions into different parts of the mapping.

Figure 49-5 uses the Linux-specific (nonstandard) *remap_file_pages()* system call, which
is useful in making nonlinear mappings. It simply swaps pages 0 and 2 of the previously
mapped file.

In my program, I created an anonymous memory mapping whose region is three system pages long.
(In my system, 4096 bytes is a page, so 12288 pages long). Then I created two temporary
two temporary files and wrote a single character to each: `A` and `B`, respectively. I
mapped file A so as to overlap the third page of the anonoymous mapping, and mapped file
B to overlap the first page of the anonymous mapping. Then I used the pointer to the
anonymous mapping region and read the first byte in each region.

To compile and run:

```bash
gcc -Wall nonlinear_memory_mapping.c
./a.out
```

The output is:

```
./a.out: Page size: 4096
./a.out: Before overlapping file mapping of temporary files over anonymous mapping
./a.out: First byte in first page (region to be overlapped): 
./a.out: First byte in third page (region to be overlapped): 
./a.out: After creating file mappings to overlap the temporary files over the anonymous mapping
./a.out: First byte in first page (overlapped region): B
./a.out: First byte in third page (overlapped region): A
```

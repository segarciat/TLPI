# Exercise 50-2

Write a program to verify the operation of the *madvise()* `MADV_DONTNEED` operation for
a writable `MAP_PRIVATE` mapping.

## Solution

According to Sectionn 50.4, the *madvise()* system call is used to improve the performance of an
application by informating the kernel about the calling process's likely usage of the pages
in the range of a virtual memory region.

`MADV_DONTNEED` is one of the valid *advice* values specified on a call to *madvise()*, and
its behavior varies across UNIX implementations. On Linux, for a `MAP_PRIVATE` region, the mapped
pages are explicitly discarded, which means that modifications to the pages are explicitly discarded,
and hence the modifications are lost. The virtual memory range remains accessible, and the next
access of each page results in a page fault, thereby reinitializing the page, either with the
contents from the file from which it was mapped, or with zeros for an anonymous mapping.

My program accepts a file as a command-line argument. It creates a private file mapping, meaning
that any changes to it are not carried through to the underlying file. The program then displays
at most 5 characters from the input file, and then uses the second command-line argument
to replace these characters in the private file mapping. Then, the *madvise()* system call
is employed with the `MADV_DONTNEED` argument, causing the kernel to discard any changes
made to the virtual memory region. After this, the contents of the region are displayed
again, which are the original contents.

```bash
gcc -Wall madvise_dontneed_private.c
./a.out README.md "# Practice"
```

The following sample output results:

```
Read: # Exercise 50-2

Write a program

Overwrote with: #Practicee 50-2

Write a program

Discarding modifications with madvise() MADV_DONTNEED
Final contents: # Exercise 50-2

Write a program
```

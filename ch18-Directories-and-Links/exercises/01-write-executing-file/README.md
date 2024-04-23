# Exercise 18.1

In Section 4.3.2, we noted that it is not possible to open a file for writing
if it is currently being executed (`open()` returns -1, with `errno` set to `ETXTBSY`).
Nevertheless, it is possible to do the following from the shell:

```bash
cc -o longrunner longrunner.c
./longrunner		# Leave running in background
vi longruner.c		# Make some changes to the source code
cc -o longrunner longrunner.c
```

The last command overwrites the existing executable of the same name. How is this
possible? (For a clue, use `ls -li` to look at the i-node of the executable file after
each compiltion.)

## Solution

The i-node after the first call to `cc` and the second are distinct, so the executable
refers to two different files. This means the old file was not written. Rather, a new
file was created and the directory entry was overwritten so that the same name entry
would be used to point to the new file; the old file is deleted in the process. As mentioned
in 18.3, an open file is only deleted whe all file descriptors are closed. Therefore, even
though the name entry is removed from the directory, its contents are still loaded in memory
as the background process executes, holding open its file descriptor.

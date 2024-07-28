# Exercise 49-2

Rewrite the programs in Listing 48-2 (`svshm_xfr_writer.c`, page 1003) and Listing 48-3
(`svshm_xfr_reader.c`, page 1005) to use a shared memory mapping instead of System V shared
memory.

## Solution

In deciding what type of memory mapping to use, we narrow it down to a shared anonoymous mapping
or a shared file mapping. The former is inappropriate because it only allows for IPC between
related process, meaning that they one is spawned from the other through *fork()*. Therefore,
we must use a shared file mapping.

At first we might consider providing the file descriptor for standard input to *mmap()*, since
the program is meant to read text from standard input. However, the standard input of the reader
is distinct from that of the writer, so the reader will not be able to access the information
this way. Instead, in replacing the System V shared memory segment by a shared file mapping,
we can replace the System V IPC key used to create the memory segemnt by a file name used by
the program, created under suitable permissions. This name is declared in the header file, and
created by the writer with suitable permissions on startup.

```bash
make mmap_xfr_writer
make mmap_xfr_reader
./mmap_xfr_writer < /etc/services &
./mmap_xfr_reader > out.txt
diff /etc/services out.txt
echo $?
```

The output of this shell session was:

```
Received 12813 bytes (13 xfrs)
Sent 12813 bytes (13 xfrs)
[1]+  Done                    ./mmap_xfr_writer < /etc/services
```

The `diff` command had no output and its exit status was 0, implying that there were no different in
the input and output file. Hence the writer correctly "piped" the information to the reader using the
shared file mapping.

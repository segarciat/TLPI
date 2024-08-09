# Exercise 54-1

Rewrite the programs in Listing 48-2 (`svshm_xfr_writer.c`) and Listing 48-3 (`svshm_xfr_reader.c`)
to use POSIX shared memory objects instead of System V shared memory.

## Solution

```bash
# Compile
make pshm_xfr_writer
make pshm_xfr_reader

# Run writer in the background
./pshm_xfr_writer < /etc/services &

# Copy using reader
./pshm_xfr_reader > services_copy

# Check they are the same
diff
echo $?
```

The last command should output 0 if the files are identical, which they should be.

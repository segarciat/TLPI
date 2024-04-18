# Exercise 16-1

Write a program that can be used to create or modify a *user* EA for a file (i.e.,
a simple version of *setfttr(1)*). The filename and the EA name and value should
be supplied as command-line arguments to the program.

## Solution

The implementation takes three command-line arguments: a file name, an extended
attribute name, and a corresponding value. The program the adds the name to
the user namespace `user.`; this means the user need not provide the namespace,
and if provided, it will be part of the attribute name.

### Build and Usage

```bash
make

# Create dummy file
touch tempfile

# Add attribute "nickname" with value "serg"
./setfattr tempfile nickname serg

# List (dump) all attributes
getfattr -d tempfile
```

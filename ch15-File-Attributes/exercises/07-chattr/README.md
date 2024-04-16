# Exercise 15-7

Write a simple version of the *chattr(1)* command, which modifies file i-node flags. See the
*chattr(1)* man page for details of the `chattr` command-line interface. (You don't need to
implement the `-R`, `-V`, and `-v` options.)

## Solution

Running `man 1 chattr` shows the following at the beginning of the command description:

```
chattr changes the file attributes on a Linux file system.

The format of a symbolic mode is +-=[aAcCdDeFijmPsStTux].

The  operator  '+' causes the selected attributes to be added to the existing attributes of the
files; '-' causes them to be removed; and '=' causes them to be the only  attributes  that  the
files have.
```

My implementation only provides the ability to modify the flags mentioned in the book. Namely, it
supports: `[-+=acDijAdtsSTu]`. Also it must be privilege, so users should use `sudo`.

Sample usage:

```bash
# Show attributes
lsattr

# Add append-only attribute
sudo ./chattr +a ./README.md

lsattr

# Remove the append-only attribute
sudo ./chattr -a ./README.md

# Set attributes
touch test.txt
sudo ./chattr =a test.txt

lsattr

# Use the chattr command, instead of our program, to add e attribute back
sudo chattr =e test.txt

sudo rm hello.txt
```

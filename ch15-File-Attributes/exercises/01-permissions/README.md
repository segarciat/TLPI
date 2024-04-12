# Exercise 15-1

Section 15.4 contained several statements about permissions required for
various file-system operations. Use shell commands or write programs to
verify or answer the following:

a) Removing all owner permissions from a file denies the file owner access,
even though group and other permissions do have access.

b) On a directory with read permission but not execute permission, the
names of files in the directory can be listed, but the files themselves
can't be accessed, regardless of the permissions on them.

c) What permissions are required on the parent directory and the file
in order to create a new file, open a file for reading, open a file for
writing, and delete a file? What permissions are required on the source
and target directory to rename a file? If the target file of a rename
operation already exists, what permissions are required on that file?
How does setting the sticky permission bit (`chmod +t`) of a directory
affect renaming and deletion operations?

## Solution

a) I created a file `no-owner-permissions.txt` and changed its permissions
with `chmod`, then attempted to read it with `cat`:

```bash
touch `no-owner-permissions.txt
chmod 044 no-owner-permissions.txt
cat no-owner-permissions.txt
```

The result was:

```
cat: no-owner-permissions.txt: Permission denied
```

b) I created a directory `no-execute-dir` with a file `hello.txt` and subdirectory `animals`. I listed its files, and attempted to see the contents of `hello.txt`:

```bash
mkdir -p no-execute-dir/animals
echo "Hello world" > no-execute-dir/hello.txt
echo "Fish:Turtle:Starfish" > no-execute-dir/animals/aquatic.txt
chmod -x no-execute-dir

# Successfully listed animals and hello.txt
/bin/ls no-execute-dir/

# Permission denied
cat no-execute-dir/hello.txt

# Permission denied
ls no-execute-dir/animals
```

I used `/bin/ls` to avoid using the alias set on my shell. The result of
trying to list the files succeeded, but accessing the files or subdirectories
failed.

c) To create, open, write, and delete a file, we require Write and execute
permissions is necessary on the parent directory, but not read. This means we
can operate with the file but we cannot view the contents of the directory, so
`ls` will fail with *Permission denied*. We also need read and write access on
the file.

Write and execute permissions are needed on the source and destination
directories to rename a file:

```bash
mkdir source_dir
touch source_dir/file-to-move.txt
mkdir dest_dir

chmod 300 source_dir
chmod 300 dest_dir
mv sourcedir/file-to-move.txt dest_dir
```

For the target of a rename operation, permissions are not needed at all:

```bash
touch file-to-rename.txt
touch file-to-be-overwritten.txt
chmod 000 file-to-be-overwritten.txt
mv file-to-rename.txt file-to-be-overwritten.txt
``` 

and I was prompted with:
```
mv: replace 'file-to-be-ovewritten.txt', overriding mode 0000 (---------)?
```

After typing y, the replace was successful, in spite of not having permission
on the target.

If the directory has a sticky bit, then even if I have no permissions, I am able to rename the file:

```bash
mkdir dir-with-stickybit
touch dir-with-stickybit/test-file.txt
chmod 000 dir-with-stickybit/test-file.txt
mv dir-with-stickybit/test-file.txt dir-with-stickybit/renamed.txt
```

When attempting to remove the file, we are prompted for confirmation because
it is now "write-protected":

```
rm: remove write-protected regular empty file 'dir-with-stickybit/renamed.txt'?
```

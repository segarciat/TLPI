# Exercise 18.2

Why does the call to `chmod()` in the following code fail?

```c
mkdir("test", S_IRUSR | S_IWUSR | S_IXUSR);
chdir("test");
fd = open("myfile", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
symlink("myfile", "../mylink");
chmod ("../mylink", S_IRUSR);
```

## Solution

The `symlink()` system call cretes a symbolic link pointing to `"myfile"`, which
has been opened with read and write permissions for the user, and none otherwise.
The `chmod()` system call deferences the symbolic link and tries to modify the
only allow read for the user.

Note that according to 15.4.3, once a file has been opened with `open()`, no permission
checking is performed by subsequent system calls that work with the returned file
descriptor. Therefore even if `chmod()` succeeded, we would still be able to write to
the file referred to by `fd`.

The real issue is that according to Section 18.2, relative pathnames in symbolic links
are interpreted relative to the location of the link itself. The `symlink()` call therefore
creates `mylink` in the directory that is a parent of `test`, pointing to `myfile`, when
instead it should point to `test/myfile`, relative to where it is creted.

To achieve the intent, an absolute path should be provided to `symlink`. The programmer
could use the `getcwd()` function before descending into the `test` directory, then
concatenate this with `/`, `test/`, and `myfile`, yielding the absolute path to `myfile`.



permissions to only either enoent or cannot change permissions on symlink

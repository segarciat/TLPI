# Exercise 38.1

Log in as a normal, unprivileged user, create an executable file (or copy an existing file such
as `/bin/sleep`), and enable the set-user-ID permission bit on the file (`chmod u+s`). Try
modifying the file (e.g., *cat >> file*). What happens to the file permissions as a result
(`ls -l`)? Why does this happen?

## Solution

I created a simple program `hello.c` that prints:

```
Hello, world
```

I compiled it, enabled the set-user-ID permission bit, and displayed its permissions to verify
the operation succeeded:

```bash
gcc hello.c
chmod u+s ./a.out
ls -l
```

The output was:

```bash
total 24
-rwsrwxr-x 1 sgarciat sgarciat 15960 Jun 24 11:32 a.out
-rw-rw-r-- 1 sgarciat sgarciat    85 Jun 24 11:32 hello.c
-rw-rw-r-- 1 sgarciat sgarciat   328 Jun 24 11:32 README.md
```

As you can see, `a.out` has permissions `-rwsrwxr-x`, where the `s` indicates that the set-user-ID
permission bit was successfully enabled. Then I modified the contents of the file:

```bash
cat >> a.out
```

Finally I listed the file permissions once more with `ls -l`:

```bash
total 24
-rwxrwxr-x 1 sgarciat sgarciat 15960 Jun 24 11:32 a.out
-rw-rw-r-- 1 sgarciat sgarciat    85 Jun 24 11:32 hello.c
-rw-rw-r-- 1 sgarciat sgarciat   328 Jun 24 11:32 README.md
```

The permission bit was disabled. This is likely done to prevent a malicious user from modifying
the program so that it performs actions with the privilege afforded by the effective user ID
obtained through running the set-user-ID program.

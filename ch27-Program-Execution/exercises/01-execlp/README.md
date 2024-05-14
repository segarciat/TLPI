# Exercise 27.1

The final command in the following shell session uses the program in Listing 27.3
to exec the program `xyz`. What happens?

```
$ echo $PATH
/usr/local/bin:/usr/bin:/bin:./dir1:./dir2
$ ls -l dir1
total 8
-rw-r--r-- 1 mtk	users	7860	June 13 11:55 xyz
$ ls -l dir2
total 28
-rwxr-xr-x 1 mtk	users	27452	June 13 11:55 xyz
./t_execlp xyz
```

## Solution

The `execlp` uses the named program and the `PATH` environment variable to find the
location of the program to load and execute. Since `xyz` belongs to both `./dir1`
and `./dir2`, we expect that the one in `./dir1` will be found first. However it
is not executable. Therefore `execlp` will not be successful in execing that file.
As described in Section 27.2.1, `execvp()` and `execlp()` continue until a file
with the given name is successfully execed. Thus, eventually `./dir2` is reached
and the `xyz` file there, is execed because it does have executable permission enabled.

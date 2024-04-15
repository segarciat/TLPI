# Exercise 15-6

The `chmod a+rX file` command enables read permission for all categories of user, and likewise
enables execute permission for all categories of user if `file` is a directory or execute
permission is enabled for any of he user categories for `file`, as demonstrated in the following
example:

```bash
ls -ld dir file prog
```

```
dr--------	2 mtk users	   48 May	4 12:28 dir
-r--------	1 mtk users	19794 May	4 12:22 file
-r-x------	1 mtk users	19336 May	4 12:21 prog
```

```bash
chmod a+rX dir file prog
ls -ld dir file prog
```

```
dr-xr-xr-x	2 mtk users	   48 May	4 12:28 dir
-r--r--r--	1 mtk users	19794 May	4 12:22 file
-r-xr-xr-x	1 mtk users	19336 May	4 12:21 prog
```

Write a program that uses `stat()` and `chmod()` to perform the equivalent of `chmod a+rX`.

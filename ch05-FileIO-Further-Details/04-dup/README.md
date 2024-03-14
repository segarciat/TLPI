# Exercise 05-04

Implement `dup()` and `dup2()` using `fcntl()` and, where necessary, `close()`.
(You may ignore the fact that `dup2()` and `fcntl()` return different `errno`
values for some error cases.) For `dup2()`, remember to handle the special case
where `oldfd` equals `newfd`. In this case, you should check whether `oldfd`
is valid, which can be done, for example, by checking if `fcntl(oldfd, F_GETFL)`
succeeds. If `oldfd` is not valid, then the function should return `-1` with
`errno` set to `EBADF`.

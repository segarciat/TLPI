# Exercise 63-6

Write a program that uses *epoll_create()* to create an *epoll* instance and then
immediately waits on the returned file descriptor using *epoll_wait()*. When, as
in this case, *epoll_wait()* is given an *epoll* file descriptor with an empty interest
list, what happens? Why might this be useful?

## Solution

The call blocks indefinitely unless it is interrupted by a signal, or if the timeout
provided is reached. According to Kerrisk, this may be useful in a multithreaded program,
where one thread blocks waiting while a different thread adds to the interest list.

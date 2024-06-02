# Exercise 33.1

Write a program to demonstrate that different threads in the same process can have different sets
of pending signals, as returned by `sigpending()`. You can do this by using `pthread_kill()` to send
different signals to two different threads that have blocked these signals, and then have each of
the threads call `sigpending()` and display information about pending signals. (You may find the
functions in Listing 20-4 useful).

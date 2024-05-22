# Exercise 29.1

What possible outcomes might there be if a thread executes the following:

```c
pthread_join(pthread_self(), NULL);
```

Write a program to see what actually happens on Linux. If we have a variable, `tid`,
containing a thread ID, how can a thread prevent itself from making a call, 
`pthread_join(tid, NULL)`, that is equivalent to the above statement?


## Solution

The `phread_join()` function can be any executed by a thread in a process to terminate
the thread specified in the first argument. It behave ssimilar to `waitpid()` for
reaping child processes.

One possible outcome is that the thread simply exits as if it had called `return` from
its start function or `pthread_exit()` from any function called by the start function.

Another possibility is that it may become detached, as if it had called `phthread_detatch(pthead_self())`,
and continues to execute.

Yet another possibility is that it results in a segmentation error because its stack frame
becomes invalid and yet it continues to try and execute.

I wrote `pthread_join_self` to find out, which can be compiled and run as follows:

```bash
# -pthread option links against libpthread
gcc pthread_join_self -pthread
./a.out
```

Based on the output, there appeared to be no error, and the program ran as if the call
was ignored. However, Kerrisk explains that the possibilities are that it either deadlocks
or the call fails with `EDEADLK`. I missed this because I did not check the return status
of `pthread_join()` within the thread function. After modifying this I saw that it's indeed true:

The following is the output of the program:

```
Hello from main. Sleeping...
Hello world
Error in tread attempting to self join: Resource deadlock avoided

```

Kerrisk suggests we can employ the `pthread_equal()` function to guard against the possibility:

```c
if (!pthread_equal(tid, pthread_self()))
	pthread_join(tid, NULL);
```

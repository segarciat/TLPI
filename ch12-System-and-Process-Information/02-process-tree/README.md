# Exercise 12-02

Write a program that draws a tree showing the hierarchical parent-child
relationships of all processes on the system, going all the way back to
*init*. For each process, the program should display the process ID and
the command being executed. The output of the program should be similar
to that produced by *pstree(1)*, although it does not need to be as
sophisticated. The parent of each process on the system can be found by
inspecting the *PPid:* line of all the `/proc/PID/status` files on the
system. Be careful to handle the possibility that a process's parent
(and thus its `/proc/PID` directory) disappears during the scan of all
`/proc/PID` directories.

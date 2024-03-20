# Exercise 12-03

Write a program that lists all processes taht have a particular file pathname open.
This can be achieved by inspecting the contents of all the `/proc/PID/fd/*` symbolic
links. This will require nested loops employing *readdir(3)* to scan all `/proc/PID`
directories, and then the contents of all `/proc/PID/fd` entries within each `/proc/PID`
directory. To read the contents of a `/proc/PID/fd/n` symbolic link requires the use of
*readlink()*, described in Section 18.5.

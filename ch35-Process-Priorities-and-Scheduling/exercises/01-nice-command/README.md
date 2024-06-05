# Exercise 35.1

Implement the *nice(1)* command

## Solution

Running `type -a nice` reveals that it is a program and not a shell built-in on my system:

```bash
type -a nice
man nice
```

The man page entry describe it as follows:

```
SYNOPSIS
	nice [OPTION] [COMMAND [ARG]...]

DESCRIPTION
	Run  COMMAND  with an adjusted niceness, which affects process scheduling.  With no COMMAND, print the current
	niceness.  Niceness values range from -20 (most favorable to the  process)  to  19  (least  favorable
	to the process).

	Mandatory arguments to long options are mandatory for short options too.

	-n, --adjustment=N 
		add integer N to the niceness (default 10)

   --help
   		display this help and exit

   --version
		  output version information and exit
```

To achieve this, I used the `setprioty()` system call to set the priority of my program, and then
used `execvp()` to run the program provided by the user. This works because, according to Section 35.1,
the nice value is inherited by a child created via `fork()`, and preserved across an `exec()`.

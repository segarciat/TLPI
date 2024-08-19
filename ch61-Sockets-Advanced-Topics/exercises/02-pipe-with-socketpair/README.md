# Exercise 61-2

Implement *pipe()* in terms of *socketpair()*. Use *shutdown()* to ensure that the resulting pipe
is unidirectional.

## Solution

The implementation was straightforward, as can be seen from `pipe_sp.c`. I wrote a test program
`main.c` that does the same as Listing 44-2. That is, it accepts a single command-line argument,
and echoes it to standard output. Under the hood, it consists of a  process that creates a pipe
and then forks a child. The parent writes the command-line argument string to the pipe, and the
child reads it from the pipe before writing it to standard out.

```bash
gcc -Wall -o main main.c pipe_sp.c
./main 'I looked at the mirror, and looked tired. In the corner I noticed my cat staring at me, and my tiredness went away'
```

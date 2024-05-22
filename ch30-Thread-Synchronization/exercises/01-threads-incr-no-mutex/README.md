# Exercise 30.1

Modify the program in Listing 30.1 (`thread_inc.c`) so that each loop in the thread's start
function outputs the current value of `glob` and some identifier that uniquely identifies
the thread. The unique identifier for the thread could be specified as an argument to the
`pthread_create()` call used to create the thread. For this program, that would require changing
the argument of the thread's start function to be a pointer to a struct containing the unique
identifier and a loop limit value. Run the program, redirecting output to a file, and then
inspect the file to see what happens to `glob` as the kernel schedules alternate execution
between the threads.

## Solution

As suggested I created a struct to group the thread identifier and the number of times it needed
to loop:

```c
struct tharg_s {
	char *id;
	int loops;
};
```

In `main()`, I initialized both before creating the threads:

```c
	pthargs1.id = "t1";
	pthargs1.loops = loops;

	pthargs2.id = "t2";
	pthargs2.loops = loops;
```

In the start function, I casted the argument to the proper type and displayed the value
to stdout:

```c
	struct tharg_s *threadArgs = (struct tharg_s *) arg;
    int loc, j;

    for (j = 0; j < threadArgs->loops; j++) {
        loc = glob;
        loc++;
        glob = loc;
		printf("[%s]: glob=%d\n", threadArgs->id, glob);
    }
```

To compile run, run `make` (notice the `Makefile` links against `libpthread` with the `-pthread` option
of `gcc`):

```c
make
./thread_incr 1000000 > results.txt
tail results.txt
```

The expected result is 2000000, but because the threads are not synchronized, they overwrite one
another's results, as indicated by the incorrect result:

```
[t2]: glob=1920119
[t2]: glob=1920120
[t2]: glob=1920121
[t2]: glob=1920122
[t2]: glob=1920123
[t2]: glob=1920124
[t2]: glob=1920125
[t2]: glob=1920126
[t2]: glob=1920127
glob = 1920127
```

The threads are stepping over each other, as can be seen from the following inspection of the output
file:

```bash
head -1000 results.txt | tail -30 | head -10
```

```
[t2]: glob=957
[t2]: glob=958
[t2]: glob=959
[t2]: glob=960
[t2]: glob=961
[t2]: glob=962
[t2]: glob=963
[t2]: glob=964
[t1]: glob=813
[t1]: glob=966
```

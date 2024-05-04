# Exercise 23.2

Try running the program in Listing 23.3 (`t_nanosleep.c`) in the background with a 60-second
sleep interval, while using the following command to send as many `SIGINT` signals as possible
to the background process:

```bash
while true; do kill -INT pid; done
```

You should observe that the program sleeps rather longer than expected. Replace the use of
`nanosleep()` with the use of `clock_gettime()` (use a `CLOCK_REALTIME` clock) and
`clock_nanlosleep()` with the `TIMER_ABSTIME` flag. (This exercise requires Linux 2.6.)
Repeat the test with the modified program and explain the difference.

## Solution

First I built the program and ran it in the background:

```bash
make
./t_nanosleep 60 0 &
```

Which outputs the `PID`:

```
[1] 15328
```

Then I used the suggested command to send `SIGINT` signals very quickly:

```bash
while true; do kill -INT 15328; done
```

The terminal flooded with messages that informed of the time slept so far and the remaining
sleep time. However the remaining time was actually increasing:

```
...
Slept for: 64.448481 secs
Remaining: 308.367598667
Slept for: 64.448502 secs
Remaining: 308.367695257
^CSlept for: 64.448530 secs
Remaining: 308.367789089
```

Notice the `^C`, because I interrupted the infinite loop by pressing *Control+C* to send
a `SIGINT`. The problem is described in Sections 23.2 and 23.4.2. The accuracy of the sleep
interval is limited by the frequency of the software clock, so when a sleep interval value
does not exactly match the granulity of the software clock, the timer value is *rounded up*,
resulting in a roundoff error. Since the infinite loop sends `SIGINT` signals at a very
high rate, the remaining time returned in `remain` by `nanosleep()` is unlikely a multiple
of the granularity of the software clock, so when we specify it as the new sleep time,
it rounds up, causing `nanosleep()` to sleep for longer.


I proceeded to send a `SIGTERM` to the `./t_nanosleep` process running in the background:

```bash
kill -s SIGTERM 15328
```

```
[1]+  Terminated              ./t_nanosleep 60 0
```

To address the bug, we instead use the system call `clock_nanosleep()` from the POSIX API
(see `man 2 clock_nanosleep`). This procedure allows us to specify the `TIMER_ABSTIME` flag.
The requested sleep time then becomes an absolute end time as measured by the clock we use.
To take advantage of this, we will also need to use `clock_gettime()` to get the current time,
and then the absolute time will be this plus the time provided by the user on the command-line.

To use it with glibc, we can to use the feature test macro
and include:

```bash
#define _POSIX_C_SOURCE 200112L
#include <time.h>
```

Then we get the current time and add the values from the command-line arguments:
```c
	/* Establish an absolute time for sleep */
	if (clock_gettime(CLOCK_REALTIME, &request) == -1)
	    errExit("clock_gettime");
    request.tv_sec += getLong(argv[1], 0, "secs");
    request.tv_nsec += getLong(argv[2], 0, "nanosecs");
```

Then we set the timer:

```c
		s = clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &request, &remain);
		if (s != 0 && s != EINTR)
			errExitEN(s, "clock_nanosleep");
```

Notice that the `&remain` time is ignored when we use this flag.

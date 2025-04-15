# Exercise 22.2

If both a realtime and standard signal are pending for a process, SUSv3 leaves it unspecified
which is delivered first. Write a program that shows what Linux does in this case. (Have the
program set up a handler for all signals, block signals for a period of time so that you can
send various signals to it, and then unblock all signals).

## Solution

I determined that Linux will deliver standard signals before realtime signals. The following
shell session shows what I saw:

```bash
# Compile program
make

# Run program in the background
./pending_std_rt_signals &
```

The following output shows immediately, with the output of the background program intermixed
with the shell prompt:

```
[1] 39046
$ ./pending_std_rt_signals: PID: 39046
./pending_std_rt_signals: All signals but the following are blocked:
	( 2) Interrupt
	(20) Stopped
	(32) Unknown signal 32
	(33) Unknown signal 33

./pending_std_rt_signals:Sleeping up to 30 seconds, then unblocking.
```

In a new terminal, verify the program is still running in the background
by running `ps` (or `ps xa`). Then use the `kill` command to send standard
and realtime signals:

```
kill -40 39046 # realtime signal
kill -21 39046 # standard signal
kill -50 39046 # realtime signal
kill -22 39046 # standard signal
```

The following was output from the background process:

```
Received: (21) Stopped (tty input)
Received: (22) Stopped (tty output)
Received: (40) Real-time signal 6
Received: (50) Real-time signal 16
./pending_std_rt_signals: Exiting
^C # Press Control+C
[1]+  Done                    ./pending_std_rt_signals
```

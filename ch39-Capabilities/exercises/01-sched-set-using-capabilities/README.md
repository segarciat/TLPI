# Exercise 39.1

Modify the program in Listing 35-2 (`sched_set.c`, on page 743) to use file capabilities,
so that it can be used by an unprivileged user.

## Solution

The original program, `sched_set.c` is here for reference, but I have also created its
modified version, `sched_set_caps.c`, which implements the requested change.

As described in Section 35.3.2, page 743, privileged processes (those with `CAP_SYS_NICE`)
can make arbitrary changes to the scheduling policy of any process, an unprivilieged
processes can make limited changes depending on rules affected by the value of the
`RLIMIT_RTPRIO` resource limit.

To enable an unprivilieged user to use this program, we employ file capabilities in the
following way:

1. Add the `CAP_SYS_NICE` capability to the effectively capability set of the process.
2. Compile the program and link it against *libcap*.
3. Use *setcap(8)* from the *libcap* package to add `CAP_SYS_NICE` to the executable
file's permitted capability set.

This program is similar to a set-user-ID-*root* program, but much safer because such
a program would have all capabilities enabled, whereas this one has only `CAP_SYS_NICE`.
Nevertheless, it's dangerous in the sense that it the user to change the scheduling priority
of *any* process.

See the shell interaction below, where I attempt to run the program, but initially fail because
I have not yet used `setcap` to add `CAP_SYS_NICE` to the file's capability set:

```bash
# Build (Makefile links against libcap)
make sched_set_caps

# Run a program in background whose scheduling policy we will attempt to modify
sleep 20 &
# [1] 50944

# Attempt to set the set the realtime `SCHED_RR` scheduling policy with priority 2
./sched_set_caps r 2 50944

# ERROR [EPERM Operation not permitted] raiseCap() failed

# Add `CAP_SYS_NICE` to the executable file's permitted capability set
sudo setcap "cap_sys_nice=p" sched_set_caps

# Verify change was successful
getcap sched_set_caps
# sched_set_caps cap_sys_nice=p

# Create a new process...
sleep 20 &
# [1] 51029

# ... and attempt to modify its scheduling policy. This time we succeed.
./sched_set_caps r 2 51029

# Check return status
echo $?
# 0
```

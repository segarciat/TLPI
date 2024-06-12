# Exercise 35.4

If two processes use a pipe to exchange a large amount of data on a multiprocessor system, the communication
should be faster if the processes run on the same CPU than if they run on different CPUs. The reason is that
when the two processes run on the same CPU, the pipe data will be more quickly accessed because it can remain
in that CPU's cache. By contrast, when the processes run on separate CPUs, the benefits of the CPU cache are
lost. If you have access to a multiprocessor system, write a program that uses *sched_setaffinity()* to
demonstrate this effect, by forcing the processes either onto the same CPUs or onto different CPUs (Chapter
44 desribes the use of pipes.)

> The advatange in favor of running on the same CPU won't hold true on hyperthreaded systems and on some
modern multiprocessor architectures wher the CPUs do share the cache. In these cases, the advantage will
be in favor of processes running on different CPUs. Information about the CPU topology of a multiprocessor
system can be obtained by inspecting the contents of the Linux-specific `/proc/cpuinfo` file.

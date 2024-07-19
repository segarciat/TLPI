# Exercise 47-5

For the VMS operating system, Digital provided a synchronization method similar to a binary
semaphore, called an *event flag*. An event flag  has two possible values, *clear* and *set*,
and the following four operations can be performed: *setEventFlag*, to set the flag; *clearEventFlag*,
to clear the flag; *waitForEventFlag*, to block until the flag is set; and *getFlagState*, to obtain
the current state of the flag. Devise an implementation of event flags using System V semaphores. This
implementation will require two arguments for each of the functions above: a semaphore identifier
and a semaphore number. (Consideration of the *waitForEventFlag* operation will lead you to realize
that the values chosen for *clear* and *set* are not the obvious choices.)

## Solution

Waiting for the event flag to be sounds like blocking until the semaphore has a certain value.
A `semop()` blocks while waiting for the semaphore to have the value 0, or for the semaphore to
have a nonzero value so that it can be decremented to a nonnegative value.

I presume that setting the flag is equivalent to making the semaphore value 0, and clearing it is
equivalent to making it 1. Waiting  for the semaphore to be is waiting for it to be 0.

# Exercise 63-4

The last step of the description of the self-pipe technique in Sectionn 63.5.2
stated that the program should first drain the pipe, and then perform any actions
that should be taken in response to the signal. What might happen if these steps
were reversed?

## Solution

Kerrisk describes that it can lead to a race condition. His reasoning is that
after *select()* is interrupted by a signal and the program performs the
appropriate action in response, a new signal may be delivered before the
pipe is drained, and therefore the process would not see this signal.

However, I fail to see the issue because even if the queue were to be drained
first, couldn't a signal arrive after the *select()* and before the drain,
thus leading to the same problem?

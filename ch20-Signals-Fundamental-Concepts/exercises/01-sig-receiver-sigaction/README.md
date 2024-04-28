# Exercise 20.1

As noted in Section 20.3, `sigaction()` is more portable than `signal()` for establishing a
signal handler. Replace the use of `signal()` by `sigaction()` in the program in Listing
20-7. (`sig_receiver.c`). 

## Solution

The changes made were small. First we declared a `struct sigaction` object:

```c
struct sigaction sigAction;
sigAction.sa_handler = handler;
sigAction.sa_flags = 0;
```

The object has the address of the handler to be called when the signal of
interest is caught (delivered to the process). The flags are used to specify
the set of signals that are blocked during the invocation of the handler.
Here, I set it to 0 so as to not block any signals. Then I replaced the
`signal()` call in the loop as follows:

```c
	/* ... */
	for (n = 1; n < NSIG; n++)          /* Same handler for all signals */
        (void) sigaction(n, &sigAction, NULL);      /* Ignore errors */
	/* ... */
```

Otherwise the program remains the same:

```bash
# Build
make

# Run, providing optional pause duration
./sig_receiver 15

# Process CTRL+C or send signals with kill(1) command.
# See output with signals caught
```

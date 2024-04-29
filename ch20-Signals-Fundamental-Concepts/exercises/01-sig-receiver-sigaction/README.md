# Exercise 20.1

As noted in Section 20.3, `sigaction()` is more portable than `signal()` for establishing a
signal handler. Replace the use of `signal()` by `sigaction()` in the program in Listing
20-7. (`sig_receiver.c`). 

## Solution

The changes made were small. First we declared a `struct sigaction` structure:

```c
struct sigaction sigAction;
if (sigemptyset(&sigAction.sa_mask) == -1)
	errExit("sigemptyset");
sigAction.sa_handler = handler;
sigAction.sa_flags = 0;
```

The structure has the address of the handler to be called when the signal of
interest is caught (delivered to the process). The `sa_mask` field is used to specify
the set of signals that are blocked during the invocation of the handler. By default
this is just the signal passed to the `sigaction()` function, but this field allows
specifying additional ones if desired. Here, I used the `sigemptyset()` function to
specify that no additional flags will be blocked during the procedure call. I also
set the `sa_flags` to 0, which controls the handler invocation. The 0 indicates that
no special flags are used.

Then I replaced the `signal()` call in the loop as follows:

```c
	/* ... */
	for (n = 1; n < NSIG; n++)          /* Same handler for all signals */
        (void) sigaction(n, &sigAction, NULL);      /* Ignore errors */
	/* ... */
```

Otherwise the program remains the same. I specified `NULL` in the second argument
since I was not interested in the previous disposition.

```bash
# Build
make

# Run, providing optional pause duration
./sig_receiver 15

# Process CTRL+C to send interrupt or send signals with kill(1) command.
# See output with signals caught
```

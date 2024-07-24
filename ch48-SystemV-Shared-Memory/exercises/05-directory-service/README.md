# Exercise 48-5

Write a directory service that uses a shared memory segment to publish name-value
pairs. You will need to provide an API that allows callers to create a new name,
modify an existing name, delete an existing name, and retrieve the value associated
with a name. Use semaphores to ensure that a process performing an update to the
shared memory segment has exclusive access to the segment.

## Solution

Initialize the service by running 

```bash
gcc -Wall -o shmdir_init_service shmdir_init_service.c
./shmdir_init_service
```

This program does three simple things:

1) Creates the System V semaphore used by the service.
2) Initializes the System V semaphore
3) Creates the shared memory segment used by the service.

On success, the output will be similar to the following:

```
./shmdir_init_service: Created System V semaphore with id 2
./shmdir_init_service: Created System V shared memory segment with id 32782
```

Running `ipcs -s` will display the semaphores available and `ipcs -m` will display the memory
segments.

The actual API for the "service" is in `shmdir.h`. The user is expected to call `shmdir_init()`
before doing any other interaction with the service. This simply gets the semaphore ID and base
address of the shared memory segment, both of which are needed for all furhter API usage.
Once the use is finished interacting with the service, they call `shmdir_shutdown()`, which simply
detaches the shared memory segment.

The rest of the service is implemented in the calls `shmdir_add()`, `shmdir_get()`, `shmdir_upd()`,
and `shmdir_del()`, which add, get, update, and delete name-value pairs, respectively.
Access to the segment is controlled by the semaphore.

I decoded to use an array an array of structures for the name-value pairs, where names and values
have a fixed length. My usage is neither space-efficient nor time-efficient, but it has the
virtue of simplicity. The shared memory segment has enough room for 1024 name-value pairs.

I have also provided a command-line interface corresponding to these functions which I have named
`shmdir_cli.c`:

```bash
gcc -Wall -o shmdir_cli shmdir_cli.c shmdir.c
./shmdir_cli -h
```

The help text looks like so:

```
Usage: ./shmdir_cli [{-a|-u|-d|-g} -h] name [value]
    -h        View this help text.
    -a        Add a name-value pair.
    -u        Replace a value corresponding to given name with new specified value.
    -g        Get value for given name.
    -d        Delete name-value for given name.
```

Only one of `-a`, `-u`, `-g`, or `-d` may be specified. If `-a` or `-u` is specified, then the
*value* argument is required. For example:

```bash
./shmdir_cli -a name sergio
# Returns sergio
./shmdir_cli -g name
# Returns serg
./shmdir_cli -u name serg
./shmdir_cli -d name
# Error since there is no such name (it was deleted)
./shmdir_cli -g name
```

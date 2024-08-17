# Exercise 55.10

Read the manual page of the *lockfile(1)* utility that is supplied with *procmail*.
Write a simple version of this program.

## Solution

### *procmail* and *lockfile(1)*

Running `man procmail` and `man lockfile` yielded no results, so I searched
for the page using:

```bash
apt-cache show procmail
```

Then I proceeded to install it:

```bash
sudo apt install procmail
```

This made the *lockfile(1)* utility available:

```bash
man 1 lockfile
```

The Description section says that the utility can be used to create a file that can be used
as a semaphore. The Synopsis showed the following template for the command-line interface:

```
lockfile -sleeptime | -r retries |
            -l locktimeout | -s suspend | -!  | -ml | -mu | filename ...
```

In turn:

- `-r sleeptime`: Upon failing to obtain the semaphore, this is the amount of time to wait before trying.
The default is 8 seconds.

- `-r retries`: This is the number of times the process should retry to obtain the semaphore. By default
this is `-1`, which means forever.

### My `lockfile` implementation

I decided to only implement the `-sleeptime` and `-r retries` options. Moreover, I decided to  use `-w wait_time`
instead of `-sleeptime`. Furthermore, I interpret any negative number (not just `-1`) to mean "retry forever".
Finally, instead of locking several files at once, I decided to only allow locking once.

Compile the program as follows:

```bash
gcc -Wall -o lockfile lockfile.c -lrt
```

I link against *librt* because I am using the POSIX clock options. Now run the program as follows:

```bash
./lockfile --help
```

The output is:

```
Usage: ./lockfile [OPTION]... FILE
Creates a file with the given name to be used as a semaphore.

Options:

  -w wait_time        If file cannot be locked, waits the specified number of seconds
                      before retrying. Defaults to 8 seconds.
  -r max_retries      Makes at most max_retries + 1 attempts to lock the file, sleeping
                      between attempts according to the wait_time. If max_retries is
                      negative, retries forever. Default is -1.
```

Sample usage:

```bash
./lockfile myfile.lock

# Fails after about 6 seconds with: ./lockfile: Gave up on locking mylock
./lockfile myfile.lock -w3 -l2

# 6 seconds then fails: retry at most 2 times (for a total of 3 attempts), waiting 3 seconds in between.
time ./lockfile myfile.lock -w 3 -l 2

# Make a blocking attempt
./lockfile myfile.lock
# Ctrl-C to send  SIGINT
```

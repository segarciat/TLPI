# Exercise 38.2

Write a set-user-ID-*root* program similar to the *sudo(8)* program. This program should take
command-line options and arguments as follows:

```
$ ./douser [-u user] program-file arg1 arg2 ...
```

The *douser* program executes *program-file*, with the given arguments, as thought it was run
by *user*. (If the `-u` option is omitted, then *user* should default to *root*). Before
executing *program-file*, *douser* should request the password for *user*, authenticate against
the standard password file (see Listing 8-2 on page 164), and then set all of the process user
and group IDs to the correct values for that user.

## Solution

Below is a sample shell interaction where I compile and run the program:

```bash
# Compile, linking against libcrypt to be able to use the crypt() function
gcc douser.c -lcrypt

# Make it a set-user-ID-root program
sudo chown root a.out
sudo chmod u+s a.out

# Run (using my credentials)
./a.out -u sgarciat /bin/ls
```

In my system, the root account does not have a password, so it's not possible to run a program as `root`.
To limit security problems, my implementation drops the privileged credential as soon as the program
starts. I reclaim the privileged credentials at two points:

1. When fetching the shadow password record.
2. When the process credentials are set to those requested by the user.

Moreover, very early in the program, I set `RLIMIT_CORE` to 0 to prevent the program from dumping core
if a signal is sent that would terminate the program. This would be fatal if it happened after the
user provided their password and before the program had an opportunity to erase the cleartext password
from memory.

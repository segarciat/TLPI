# Exercise 40.2

Modify the program in Listing 40-3 (`utmpx_login.c`) so that it updates the `lastlog` file in
addition to the `utmp` and `wtmp` files.

## Solution

In modifying the code, I encapsulated the `lastlog` specific code in a helper function. This
directory contains the original `utmpx_login.c`, as well as my modified version, `utmpx_login_lastlog.c.

```bash
# Build program
make utmpx_login_lastlog

# Create utmp, wtmp, and lastlog entry for my user (sleep for 5 seconds in between)
sudo ./utmpx_login_lastlog sgarciat 5
```

The output was:

```
Creating login entries in utmp and wtmp
        using pid 36151, line pts/1, id /1
Creating lastlog entry with
	using user sgarciat (1000), terminal /dev/pts/1, and hostname sgarciat-TUF-FX505GT
Creating logout entries in utmp and wtmp
```

Finally I ran `lastlog -u sgarciat` to verify the file was correctly updated:

```
Username         Port     From             Latest
sgarciat         /dev/pts sgarciat-TUF-FX5 Tue Jun 25 15:58:37 -0400 2024
```

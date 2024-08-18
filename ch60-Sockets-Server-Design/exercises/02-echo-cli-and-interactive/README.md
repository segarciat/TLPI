# Exercise 60-2

Sometiemes, it may benecessary to write a socket server so that it can be invoked
eiher directly from the command line or indirectly via *inetd*. In this case, a command-line
option is used to distinguish the two cases. Modify the program in Listing 60-4 so that,
if it is given a *-i* command-line option on the connected socket, which *inetd* supplies via
`STDIN_FILENO`. If the *-i* option is not supplied, the the program can assume it is being invoked
form the command line, and operate in the usual fashion. (This change requires only the addition
of a few lines of code.) Modify `/etc/inetd.conf` to invoke this program for the *echo* service.

## Solution

I attempted to use `inetdd`, though it required installing first:

```bash
sudo apt install openbsd-inetd
```

Unfortunately, when I attempted to configure the `/etc/inetd.conf` file and have `inetd`
reread the configuration by issuing a `SIGHUP`, I kept getting permission denied, even
while using `sudo`. Thus I was unable to invoke the *echo* service.

# Exercise 37.1

Write a program (similar to *logger(1)*) that uses *syslog(3)* to write arbitrary messages to the
system log file. As well as accepted a single command-line argument containing the message to be
logged, the program should permit an option to specify the *level* of the message.

## Solution

The *logger(1)* command accepts a message, which it outsput to `/var/log/syslog`. For example:

```bash
logger hello
tail /var/log/syslog
```

This shows

```
...
Jun 20 19:37:50 sgarciat-TUF-FX505GT sgarciat: hello
```

To specify the *level* of the message, the `-p` option is used. The man page for `logger` says:

```
...
       -p, --priority priority
           Enter the message into the log with the specified priority. The priority may be specified numerically or
           as a facility.level pair. For example, -p local3.info logs the message as informational in the local3
           facility. The default is user.notice.
...
```

The levels accepted by `logger`, according to the man page, are: `emerg`, `alert`, `crit`, `err`, `warning`, 
`notice`, `info`, `debug`, `panic` (deprecated synonym for `emerg`), `error` (deprecated synonym for `err`),
`warn` (deprecated synonym for `warning`).

I supported the non-deprecated names using `-l` (for level) instead of `-p`. Unlike *logger(1)*, which allows
the user to omit a message on the command-line arguments and to read the message a line at a time from
*stdin*, my implementation requires a message to be provided as the last command-line argument.

You can compile and run as follows:

```bash
# Compile
gcc logger.c

# Run
./a.out hello
./a.out -l info goodbye

# Verify syslog
tail /var/log/syslog
```

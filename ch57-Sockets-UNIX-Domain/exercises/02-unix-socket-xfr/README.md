# Exercise 57-2

Rewrite the programs in Listing 57-3 (`us_xfr_sv.c`) and Listing 57-4 (`us_xfr_cl.c`)
to use the Linux-specific abstract socket namespace (Section 57.6).

## Solution

The Linux abstract socket namespace is a feature that allows us to bind a UNIX domain
socket to a name withoutt hat name being created in the filesystem. To create an
abstract binding of this form, we specify the first byte in the *sun_path* field
of the `sockaddr_un` structure to be a null byte (`\0`). The name is then defined
by the remaining bytes, including any null bytes that may or may not be present.

The change to the program was small, as indicated by this `diff`:


```bash
diff -C us_xfr_cl.c us_xfr_linux_sns_cl.c
```

The truncated output looks as follows:

```
*** 35,41 ****
  
      memset(&addr, 0, sizeof(struct sockaddr_un));
      addr.sun_family = AF_UNIX;
!     strncpy(addr.sun_path, SV_SOCK_PATH, sizeof(addr.sun_path) - 1);
  
      if (connect(sfd, (struct sockaddr *) &addr,
                  sizeof(struct sockaddr_un)) == -1)
--- 35,41 ----
  
      memset(&addr, 0, sizeof(struct sockaddr_un));
      addr.sun_family = AF_UNIX;
!     strncpy(&addr.sun_path[1], SV_SOCK_PATH, sizeof(addr.sun_path) - 1);
  
      if (connect(sfd, (struct sockaddr *) &addr,
                  sizeof(struct sockaddr_un)) == -1)
```

The *memset()* call  ensures that all bytes in the `sun_path` array (particularly the first
one) are null bytes, justifying copying the string starting at index 1, as indicated by the
`&addr.sun_path[1]` argument to *strncpy()*. Notice that in the case of relying on the
Linux abstract socket namespace, `sun_path` need not be null-terminated.

To actually run the program, first compile:

```bash
make us_xfr_linux_sns_cl
make us_xfr_linux_sns_sv
```

Then run test it as follows:

```bash
# Redirect server output to file and run in background
./us_xfr_linux_sns_sv > xfr_sv_output &
# [1] 20570

# Create test file
cat *.c > test_input_file

# Run client
./us_xfr_linux_sns_cl < test_input_file

# Check diff
diff test_input_file xfr_sv_output
echo $?
```

The `diff` command should have no output, and `echo $?` should yield `0`, meaning
there are no differences. You're free to clean up now:

```bash
kill 20570
rm test_input_file xfr_sv_output
```



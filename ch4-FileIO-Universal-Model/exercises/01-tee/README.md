# Exercise 04-01

The `tee` command reads its standard input until end-of-file, writing a copy
of the input to standard output and to the file named in its command-line
argument. Implement `tee` using I/O system calls. By default, `tee` overwrites
any existing file with the given name. Implement the `-a` command-line option
(`tee -a file`), which causes `tee` to append text to the end of a file if it
already exists.

## Solution

The implementation that I have given here fails-fast if an invalid option is
provided, or if the user fails to provide exactly 1 argument, the filename.

```bash
# Build program according to instructions in makefile; creates file `tee`
make

# Display "greetings" in stdout and create file with the word "greetings"
echo "greetings" | ./tee hello.txt

# Display "hi" in stdout and append to hello.txt
echo "hi" | ./tee -a hello.txt

# Display contents of hello.txt, which will include both "greetings" and "hello"
cat hello.txt

# Display "hi there", truncate content in hello.txt, and write "hi there" to it
echo "hi there" | ./tee hello.txt

# Displays "hi there"
cat hello.txt
```

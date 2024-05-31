# TLPI

This directory consists of my self-study journey through the book *The Linux Programming Interface*
by Michael Kerrisk. See [the book website here](https://man7.org/tlpi/).

## TLPI Source Code

The [source code is available through the book site](https://man7.org/tlpi/code/index.html),
provided on the website as a compressed tarball:

```bash
# Download the tarball for Distributed Version (see website)
wget https://man7.org/tlpi/code/download/tlpi-240109-dist.tar.gz

# Decompress and extract tarball in current directory
tar -xvz -f tlpi-240109-dist.tar.gz 
```

The `README` and `BUILDING` in `tlpi-dist` explain how to proceed. Specifically,
after installing the necessary packages (explained in `BUILDING`), one can run
`make` on the top-level directory `tlpi-dist`:

```bash
cd tlpi-dist
# Read README and BUILDING files to learn what packages must be installed.

# For example, it says we need libcap-dev, among others.
sudo apt install libcap-dev
sudo apt install libacl1-dev
sudo apt install libcrypt-dev
sudo apt install libselinux1-dev
sudo apt install libseccomp-dev

# Install gcc and make
sudo apt install gcc
sudo apt install make

# Build tlpi-dist/lib and all examples.
make
```

## Using the TLPI Lib

The book explains that the examples throughout the it use a homegrown library, consisting of functions
for error handling and utilities for reading integers from stdin. The single header `"tlpi_hdr.h"`
contains all that is necessary. Sometimes, it is helpful for exercises to reuse this library,
and we can do so by linking against the library (which was built in the prior step), and then using a
`makefile` in the directory of interest. Below is a template `makefile` that I use throughout exercises:

```make
P=prog_name
TLPI_LIB_PATH=../../../TLPI/tlpi-dist # Or update path relative to where makefile will be
OBJECTS=
CFLAGS= -I$(TLPI_LIB_PATH)/lib -g -Wall -std=gnu11
LDLIBS= -L$(TLPI_LIB_PATH) -l:libtlpi.a
CC=gcc

$(P): $(OBJECTS)
```

Be sure to update `prog_name`, and if necessary, `TLPI_LIB_PATH`. My default path assumes that
an exercise file that uses it will be in a directory `TLPI/chxx-chaptername/xx-exercise-name/file.c`.

## PThreads

Notice that the Pthreads man pages come in a separate package:

```bash
sudo apt install manpages-posix-dev
```

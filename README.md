# TLPI

This directory consists of my self-study journey through the book *The Linux Programming Interface*
by Michael Kerrisk. See [the book website here](https://man7.org/tlpi/).

## TLPI Soure Code

The [source code is available through the book site](https://man7.org/tlpi/code/index.html)
, provided on the website as a compressed tarball:

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

# Build tlpi-dist/lib and all examples.
make
```

## Using the TLPI Lib

The book explains that the examples throughout the book use a library consisting of functions
for error handling and utilities for reaidng integers from stdin. The single header `tlpi_hdr.h"`
contains all that is necessary. Sometimes it is helpful for exercises to reuse this library,
and we can do so by linking against the library built in the prior step by using a `makefile`
in the directory of interest:

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

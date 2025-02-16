# Overrides default shell
SHELL = /bin/bash

# Path to library provided with source code distribution of book.
TLPI_LIB_PATH = ../../../../TLPI/tlpi-dist

# C compiler to be used.
CC = gcc

# C compilation flags.
override CFLAGS += -Og -Wall -Werror -Wextra -Wconversion -Wwrite-strings -Wcast-qual \
			-std=c11 -pedantic -I$(TLPI_LIB_PATH)/lib

# Include the folder with source code distribution in the library search path.
LDFLAGS = -L$(TLPI_LIB_PATH)

# Use the static library provided with source code distribution of book.
LDLIBS = -l:libtlpi.a

# Name of executable to create
PROGRAM = main
OBJECTS = $(PROGRAM).o set_unset_env.o

# Use implicit C compilation and linking rules to compile
$(PROGRAM): $(OBJECTS)

set_unset_env.o: set_unset_env.h

.PHONY: clean
clean:
	-rm $(OBJECTS) $(PROGRAM)

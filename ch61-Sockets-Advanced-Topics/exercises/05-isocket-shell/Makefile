SHELL = /bin/sh

# Clear out suffix list
.SUFFIXES:
# Suffixes subject to implicit rules
.SUFFIXES: .c .o

srcdir ::= .
# TODO: Update program path if necessary
tlpi_lib_path ::= $(srcdir)/../../../../TLPI/tlpi-dist
tlpi_hdr_include ::= $(tlpi_lib_path)/lib

CC ::= gcc
CFLAGS += -g
# Allows user to override CFLAGS
ALL_CFLAGS = -I$(tlpi_hdr_include) -Wall -O1 -std=gnu11 $(CFLAGS)
LDFLAGS = -L$(tlpi_lib_path)
LDLIBS = -l:libtlpi.a

$(program):

%: %.c
	$(CC) $(ALL_CFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@
is_shell_sv: is_shell.h
is_shell_cl: is_shell.h

.PHONY: clean
clean:
	-rm is_shell_sv is_shell_cl

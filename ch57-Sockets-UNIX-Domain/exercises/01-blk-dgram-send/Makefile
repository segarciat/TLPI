SHELL = /bin/sh

# Clear out suffix list
.SUFFIXES:
# Suffixes subject to implicit rules
.SUFFIXES: .c .o

# TODO: Update program name
program =
srcdir = .
# TODO: Update program path if necessary
tlpi_lib_path = $(srcdir)/../../../../TLPI/tlpi-dist
tlpi_hdr_include = $(tlpi_lib_path)/lib

CC = gcc
CFLAGS = -g
# Allows user to override CFLAGS
ALL_CFLAGS = -I$(tlpi_hdr_include) -Wall -O1 -std=gnu11 $(CFLAGS)
LDFLAGS = -L$(tlpi_lib_path)
LDLIBS = -l:libtlpi.a -lrt

$(program):

%: %.c
	$(CC) $(ALL_CFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@
sun_dgram_sv: errorexit.o
sun_dgram_cl: errorexit.o

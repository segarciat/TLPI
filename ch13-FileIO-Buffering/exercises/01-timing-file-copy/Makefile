SHELL = /bin/sh
# Clear suffxies and introduce suffices subject to implicit rules
.SUFFIXES:
.SUFFIXES: .c .o

TLPI_LIB_PATH=../../../../TLPI/tlpi-dist
OBJECTS = copy.o sync_copy.o
override CFLAGS += -I$(TLPI_LIB_PATH)/lib -g -Wall -std=gnu11
LDLIBS= -L$(TLPI_LIB_PATH) -l:libtlpi.a
CC=gcc

sync_copy.o:
	$(CC) -o sync_copy $(CFLAGS) $(LDLIBS)
copy.o:
	$(CC) -o copy $(CFLAGS) $(LDLIBS)
	
	
# $(P): $(OBJECTS)

# Do not confuse clean for a while, and continue in spite of errors from rm
.PHONY : clean
clean:
	-rm copy sync_copy $(OBJECTS)

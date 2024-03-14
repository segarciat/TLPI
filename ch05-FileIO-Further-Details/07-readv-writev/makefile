P=t_readv
TLPI_LIB_PATH=../../../TLPI/tlpi-dist
OBJECTS= scatter_gather_io.o
CFLAGS= -I$(TLPI_LIB_PATH)/lib -g -Wall -std=gnu11
LDLIBS= -L$(TLPI_LIB_PATH) -l:libtlpi.a
CC=gcc

$(P): $(OBJECTS)

####################################################
# Sanity Checks
ifeq ($(CC),9c)
     $(error Use mk to enable plan9port features)
endif
ifeq ($(LD),9l)
     $(error Use mk to enable plan9port features)
endif
####################################################
OBJS = ArchitectureDetection.o OSDetection.o \
		 HardwareDetection.o Platform.o

.c.o :
	$(CC) -c $(CFLAGS) -std=c99 -Wall -Wundef -Wpointer-arith -Wshadow -Wcast-qual \
	    -Wcast-align -Winline -Wmissing-declarations -Wredundant-decls \
	    -Wmissing-prototypes -Wnested-externs \
	    -Wstrict-prototypes -Waggregate-return -Wno-implicit -I$(srcroot) $<

.PHONY: all
all: $(OBJS)

ArchitectureDetection.o: ArchitectureDetection.c ArchitectureDetection.h

OSDetection.o: OSDetection.c OSDetection.h

HardwareDetection.o: HardwareDetection.c HardwareDetection.h

Platform.o: Platform.c Platform.h

.PHONY: clean
clean:
	rm -f *.o

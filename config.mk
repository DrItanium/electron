CC ?= cc
OUTPUT ?= electron
PREFIX ?= /usr/local
LDFLAGS ?= -lm -lrt -lwiringPi -L/usr/local/lib -lwiringPiDev
CFLAGS ?= -Os -I/usr/local/include 


# Makefile for phoon

BINDIR =	/usr/local/bin
MANDIR =	/usr/local/man/man1

DEFINES =	-D_GNU_SOURCE

CC =		cc
CFLAGS =	-O -std=c99 $(DEFINES) -Wpedantic -Wall -Wextra

all:		phoon

phoon:		phoon.o date_parse.o astro.o
	$(CC) $(CFLAGS) -o phoon phoon.o date_parse.o astro.o -lm

.c.o:
	$(CC) $(CFLAGS) -c $<

date_parse.o:
astro.o:	astro.h

install:	all
	rm -f $(BINDIR)/phoon
	cp phoon $(BINDIR)
	rm -f $(MANDIR)/phoon.1
	cp phoon.1 $(MANDIR)

clean:
	rm -f phoon *.o a.out core

test: phoon
	./phoon.test

CC=gcc
PROG=	moontool
CFILES= moontool.c
OFILES= moontool.o
LIBS=	-lm

CFLAGS=-O2 -Wall -Wextra -Wpedantic --std=c99 -D_GNU_SOURCE
LDFLAGS= -g

all: $(PROG)

$(PROG):
	$(CC) moontool.c astro.c $(CFLAGS) -o $@ $(LIBS)

clean:
	rm -f $(PROG) *.o

tests: $(PROG)
	./moontool.test

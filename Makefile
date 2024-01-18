#   Make instructions for moon tool

PROG=	moontool
CFILES= moontool.c
OFILES= moontool.o
LIBS=	-lm

CFLAGS=-O2 -Wall -Wextra -Wpedantic
LDFLAGS= -g

all:	$(PROG)

$(PROG): clean
	$(CC) -lm -O2 -c astro.c
	$(CC) moontool.c astro.o $(CFLAGS) -o $@ $(LIBS)

clean:
	rm -f $(PROG) *.o

tests: $(PROG)
	./moontool.test

.POSIX:
.PHONY: all clean test install full

PREFIX =  /usr/local
BINDIR =	$(PREFIX)/bin
MANDIR =	$(PREFIX)/man/man1

STDFLAGS = --std=c99 -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE
WARNFLAGS= -Wall -Wextra -Wpedantic
OPTFLAGS = -O2 -flto
CFLAGS   = $(OPTFLAGS) $(STDFLAGS) $(WARNFLAGS) -lm

APPS   = moontool phoon
COMMON = astro.o date_parse.o
TESTFILES = $(wildcard *.test)
.PHONY: ${TESTFILES}

CC = cc
Q = @


all: ${APPS}

full: clean all test

${APPS}: ${COMMON}
	$(Q)printf "CC %-12s -> $@\n" "$@.c"
	$(Q)$(CC) $(CFLAGS) -o $@ $@.c $^

%.o: %.c
	$(Q)printf "CC %-12s -> $@\n" "$<"
	$(Q)$(CC) $(CFLAGS) -c $<

install: all
	cp phoon $(BINDIR)
	cp phoon.1 $(MANDIR)

clean:
	rm -f ${APPS} *.o a.out core

test: ${TESTFILES}

${TESTFILES}:
	./$@

.POSIX:
.PHONY: all clean test install full

PREFIX =  /usr/local
BINDIR =	$(PREFIX)/bin
MANDIR =	$(PREFIX)/man/man1

STDFLAGS = -std=c99 -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE
WARNFLAGS= -Wall -Wextra -Wpedantic
OPTFLAGS = -O2 -flto
CFLAGS   = $(OPTFLAGS) $(STDFLAGS) $(WARNFLAGS)

APPS   = mprintf phoon
COMMON = astro.o date_parse.o
TESTFILES = $(wildcard *.test)
.PHONY: ${TESTFILES}

Q = @

all: ${APPS}

full: clean ${APPS} test

${APPS}: % : ${COMMON} %.o
	$(Q)printf "CC %-12s -> $@\n" "$@.o"
	$(Q)$(CC) $(CFLAGS) -lm -o $@ $^

%.o: %.c
	$(Q)printf "CC %-12s -> $@\n" "$<"
	$(Q)$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f ${APPS} *.o a.out core

test: ${APPS} ${TESTFILES}

${TESTFILES}: ${APPS} testing.sh
	./$@

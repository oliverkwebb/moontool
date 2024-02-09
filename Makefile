.POSIX:
STDFLAGS = -std=c99 -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE
WARNFLAGS= -Wall -Wextra -Wpedantic
OPTFLAGS = -O2 -flto
CFLAGS   = $(OPTFLAGS) $(STDFLAGS) $(WARNFLAGS) $(EXFLAGS)

OBJDIR   = obj
TESTDIR  = test
APPS   = mprintf phoon globe
COMMON = $(addprefix $(OBJDIR)/, astro.o date_parse.o)
ifdef BENCH
	COMMON := $(COMMON) $(OBJDIR)/bench.o
endif
TESTFILES = $(wildcard $(TESTDIR)/*.test)
.PHONY: ${TESTFILES} all clean test full

Q = @
ifndef Q
	P = true
endif

all: obj ${APPS}

obj:
	mkdir -p obj

full: clean ${APPS} test

${APPS}: % : ${COMMON} $(OBJDIR)/%.o
	@$(P) printf "CC %-12s -> $@\n" "$@.o"
	$(Q)$(CC) $(CFLAGS) -lm -o $@ $^

$(OBJDIR)/%.o: %.c
	@$(P) printf "CC %-12s -> $@\n" "$<"
	$(Q)$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f ${APPS} $(OBJDIR)/*.o a.out core

test: ${APPS} ${TESTFILES}

${TESTFILES}: ${APPS} testing.sh
	$(SH) ./$@

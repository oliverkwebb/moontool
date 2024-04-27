.POSIX:
STDFLAGS = -std=c99 -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE -D_DEFAULT_SOURCE
WARNFLAGS= -Wall -Wextra -Wpedantic
OPTFLAGS = -O2 -flto
CFLAGS   = $(OPTFLAGS) $(STDFLAGS) $(WARNFLAGS) $(EXFLAGS)
Q = @
OBJDIR   = obj
TESTDIR  = test
APPS   = mprintf phoon globe timecalc
COMMON = $(addprefix $(OBJDIR)/, astro.o date_parse.o)
TESTFILES = $(wildcard $(TESTDIR)/*.test)
.PHONY: ${TESTFILES} all clean test full

all: obj ${APPS}

obj:
	mkdir -p obj

full: clean ${APPS} test

${APPS}: % : ${COMMON} $(OBJDIR)/%.o
	@printf "CC %-12s -> $@\n" "$@.o"
	$(Q)$(CC) $(CFLAGS) -lm -o $@ $^

$(OBJDIR)/%.o: %.c
	@printf "CC %-12s -> $@\n" "$<"
	$(Q)$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f ${APPS} $(OBJDIR)/*.o a.out core

test: ${APPS} ${TESTFILES}

${TESTFILES}: ${APPS} testing.sh
	$(SH) ./$@

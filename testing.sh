#!/bin/sh
# Simple test harness infrastructure
#
# Copyright 2005 by Rob Landley

# "How do I use this?"
# Create a POSIX Shell script file, sourcing this one,
# set CMDNAME and CMDPATH
# run testcmd functions

#A template:

#!/bin/sh
# Toybox Test Suite, Fist Authored by Rob Landley for Toybox <https://www.landley.net/toybox>
#. ./testing.sh
## testing "name" "command" "result" "infile" "stdin"
#CMDNAME="cmd" CMDPATH="./cmd"
# 
#testcmd "display name" "command (autoprefixed with CMDPATH on testcmd)" "exected resault" "infile" "stdin"

# This file defines 2 main functions: "testing", "testcmd"

# The following environment variables enable optional behavior in "testing":
#    DEBUG - Show every command run by test script.
#    VERBOSE - "all"    continue after failed test
#              "fail"   show diff and stop at first failed test
#              "nopass" don't show successful tests
#              "quiet"  don't show diff -u for failures
#              "spam"   show passing test command lines
#
# The "testcmd" function takes five arguments:
#	$1) Description to display when running command
#	$2) Command line arguments to command
#	$3) Expected result (on stdout)
#	$4) Data written to file "input"
#	$5) Data written to stdin
#
# The "testing" function is like testcmd but takes a complete command line
# (I.E. you have to include the command name.) The variable $C is an absolute
# path to the command being tested, which can bypass shell builtins.
#
# The exit value of testcmd is the exit value of the command it ran.
#
# The environment variable "FAILCOUNT" contains a cumulative total of the
# number of failed tests.

# [ -n "$BASH" ] && shopt -s xpg_echo

export FAILCOUNT=0
: "${SHOWPASS:=PASS}" "${SHOWFAIL:=FAIL}" "${SHOWSKIP:=SKIP}" "${TESTDIR:=$(mktemp -d)}"
[ "${TESTDIR#*tmp.}" != "${TESTDIR}" ] && trap "rm -rf $TESTDIR" 0
if tty -s <&1
then
  SHOWPASS="$(printf "\033[1;32m%s\033[0m" "$SHOWPASS")"
  SHOWFAIL="$(printf "\033[1;31m%s\033[0m" "$SHOWFAIL")"
  SHOWSKIP="$(printf "\033[1;33m%s\033[0m" "$SHOWSKIP")"
fi

# Helper functions

# Check if VERBOSE= contains a given string. (This allows combining.)
verbose_has() {
  [ "${VERBOSE#*"$1"}" != "$VERBOSE" ]
}

wrong_args() {
  if [ $# -ne 5 ]
  then
    printf "%s\n" "Test $NAME has the wrong number of arguments ($# $*)" >&2
    return 1
  else
    return 0
  fi
}

# Announce success
do_pass() {
  verbose_has nopass || printf "%s\n" "$SHOWPASS: $NAME"
}

# Announce failure and handle fallout for txpect
do_fail() {
  FAILCOUNT=$((FAILCOUNT+1))
  printf "%s\n" "$SHOWFAIL: $NAME"
  if [ -n "$CASE" ]
  then
    echo "Expected '$CASE'"
    echo "Got '$A'"
  fi
  ! verbose_has all && exit 1
}

# Functions test files call directly

# Takes five arguments: "name" "command" "result" "infile" "stdin"
testing() {
  wrong_args "$@" || { printf 'testing "name" "command" "result" "infile" "stdin"'; exit; }

  [ -z "$1" ] && NAME="$2" || NAME="$1"
  [ "${NAME#"$CMDNAME "}" = "$NAME" ] && NAME="$CMDNAME $1"

  [ -n "$DEBUG" ] && set -x

  if [ -n "$SKIP" ]
  then
    verbose_has quiet || printf "%s\n" "$SHOWSKIP: $NAME"
    return 0
  fi

  echo -n "$3" > "$TESTDIR"/expected
  [ -n "$4" ] && echo -n "$4" > "$TESTDIR"/input || rm -f "$TESTDIR"/input
  echo -n "$5" | ${EVAL:-eval} "$2" > "$TESTDIR"/actual
  RETVAL=$?

  # Catch segfaults
  [ $RETVAL -gt 128 ] &&
    echo "exited with signal (or returned $RETVAL)" >> "$TESTDIR"/actual
  DIFF="$(cd "$TESTDIR" && diff -au${NOSPACE:+w} expected actual 2>&1)"
  [ -z "$DIFF" ] && do_pass || VERBOSE=all do_fail
  if ! verbose_has quiet && { [ -n "$DIFF" ] || verbose_has spam; }
  then
    [ -n "$4" ] && printf "%s\n" "echo -n \"$4\" > input"
    printf "%s\n" "echo -n '$5' |$EVAL $2"
    [ -n "$DIFF" ] && printf "%s\n" "$DIFF"
  fi

  [ -n "$DIFF" ] && ! verbose_has all && exit 1
  rm -f "$TESTDIR"/input "$TESTDIR/"expected "$TESTDIR"/actual

  [ -n "$DEBUG" ] && set +x

  return 0
}

# Wrapper for "testing", adds command name being tested to start of command line
testcmd() {
  testing "${1:-$CMDNAME $2}" "$CMDPATH"" $2" "$3" "$4" "$5"
}

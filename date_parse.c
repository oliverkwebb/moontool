/* date_parse - parse string dates into internal form
**
** Copyright (c) 1995 by Jef Poskanzer <jef@mail.acme.com>.
** All rights reserved.
**
** See LICENSE

** For a while now I've been somewhat unhappy with the various
** date-parsing routines available - yacc-based, lex-based, ad-hoc.  Large
** code size and not very portable are the main complaints.  So I wrote a
** new one that just does a bunch of sscanf's until one matches.  Slow,
** but small and portable.  To figure out what formats to support I did a
** survey of Date: lines in a bunch of Usenet articles.  The following
** two formats accounted for more than 99% of all articles:
**     DD mth YY HH:MM:SS ampm zone
**     wdy, DD mth YY HH:MM:SS ampm zone
** I added Unix ctime() format and a few others:
**     wdy, mth DD HH:MM:SS ampm zone YY
**     HH:MM:SS ampm zone DD mth YY
**     DD mth YY
**     HH:MM:SS ampm
**     DD/mth/YYYY:HH:MM:SS zone  (httpd common log format date)
** No-zone, no-seconds, and no-am/pm versions of each are also supported.
** Note that dd/mm/yy and mm/dd/yy are NOT supported - those formats are
** dumb.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

extern long timezone;

time_t date_parse(char *str)
{
  tzset();

  int fmtindx = 0;
  struct tm tlm = {0};

  char *vformats[] = {
    "%e-%b-%Y",
    "%e/%b/%y:%T %Z",
    "%e/%m/%Y",
    "%e-%b-%y %r %Z",
    "%e-%b-%y %I:%M %p %Z",
    "%e-%b-%y %r",
    "%e-%b-%y %I:%M %p",
    "%e %b %y %r %Z",
    "%e %b %y %I:%M %p %Z",
    "%e %b %y %r",
    "%e %b %y %I:%M %p",
    "%e %b %r",
    "%e %b %I:%M %p",
    "%r %Z %e-%b-%y",
    "%I:%M %p %Z %e-%b-%y",
    "%r %e-%b-%y",
    "%I:%M %p %e-%b-%y",
    "%r %Z %e %b %y",
    "%I:%M %p %Z %e %b %y",
    "%r %e %b %y",
    "%I:%M %p %e %b %y",
    "%a, %e-%b-%y %r %Z",
    "%a, %e-%b-%y %I:%M %p %Z",
    "%a, %e-%b-%y %r",
    "%a, %e-%b-%y %I:%M %p",
    "%a, %e %b %y %r %Z",
    "%a, %e %b %y %I:%M %p %Z",
    "%a, %e %b %y %r",
    "%a, %e %b %y %I:%M %p",
    "%a, %b %e %r %Z %y",
    "%a, %b %e %I:%M %p %Z %y",
    "%a, %b %e %r %y",
    "%a, %b %e %I:%M %p %y",
    "%b %e %r",
    "%b %e %I:%M %p",
    "%e %b %Y",
    "%R",
    "%r %p",
    "%s",
    NULL
  };

  for (; !strptime(str, vformats[fmtindx], &tlm); fmtindx++)
    if (!vformats[fmtindx + 1])
      dprintf(2, "Unknown date format: `%s`\n", str), exit(5);

  time_t t = mktime(&tlm) - timezone;

  // Unixtime breaks without this
  if (!vformats[fmtindx + 1]) t += timezone;

  return t;
}

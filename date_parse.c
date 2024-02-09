// date_parse - parse string dates into internal form
// See LICENSE

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static char *formats[] = {
  // d/m/y format
  "%d/%m/%Y %T",
  "%d/%m/%Y",
  // d-m-y format
  "%d-%b-%Y %T",
  "%d-%b-%Y",
  // DD MM YY HH:MM:SS
  "%d %b %Y %r",
  "%d %b %Y %T",
  "%d %b %Y %H:%M",
  "%d %b %Y",
  // ctime(3) format - zonename
  "%a %b %d %T %Y",
  "%a %b %d",
  NULL
};

static char *ifmts[] = {
  "%r",
  "%T",
  "%H:%M",
  "%b %d %T",
  "%b %d",
  NULL
};

extern time_t microtime();

time_t date_parse(char *str)
{
  if (*str == '@')
    return atol(str + 1);

  extern long timezone;
  tzset();

  int indx = 0;
  static struct tm tm; // static = bss = zeroed

  if ((*str == '+' || *str == '-') && (str[1] != '+' && str[1] != '-')) {
    time_t now = time(0), epoch = 0;

    tm = *gmtime(&epoch);

    while (!strptime(str+1, ifmts[indx], &tm))
      if (!ifmts[++indx])
        dprintf(2, "Unknown date format: `%s`\n", str), exit(2);

    now += (*str == '+') ? (mktime(&tm) - timezone) : -(mktime(&tm) - timezone);

    printf("%s", ctime(&now));
    return now;
  }

  while (!strptime(str, formats[indx], &tm) && formats[++indx])
    ;

  // ABANDON ALL HOPE; YE WHO ENTER HERE
  // Initiliaze tm after this so we don't show stuff for the year 0 a.d.

  if (!formats[indx]) {
    time_t now = time(0);
    struct tm *tmn = localtime(&now);
    tm = *tmn;
    indx = tm.tm_hour = tm.tm_min = tm.tm_sec = 0;

    while (!strptime(str, ifmts[indx], &tm))
      if (!ifmts[++indx])
        dprintf(2, "Unknown date format: `%s`\n", str), exit(2);

    return mktime(&tm);
  }

  // printf("%s", asctime(&tm));
  return mktime(&tm) - timezone;
}

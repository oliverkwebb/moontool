#include <stdio.h>
#include <time.h>

extern time_t date_parse(char *str);

int main(int argc, char **argv)
{
  if (argc < 4)
    return 1;

  time_t a1 = date_parse(argv[1]);
  time_t a2 = date_parse(argv[3]);

  a1 += (*argv[2] == '+') ? a2 : -a2;

  struct tm tm = *((argc > 5) ? localtime : gmtime)(&a1);
  tm.tm_mday--;
  tm.tm_mon--;

  char buf[100];
  strftime(buf, 100, (argc > 4) ? argv[4] : "%m %d %T", &tm);

  puts(buf);
}

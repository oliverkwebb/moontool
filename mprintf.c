/* A Moon for the Unix Machine, Release 3.0

 This is a program which displays the current phase of the Moon.

    The algorithms used in this program to calculate the positions Sun and
    Moon as seen from the Earth are given in the book "Practical Astronomy
    With Your Calculator" by Peter Duffett-Smith, Second Edition,
    Cambridge University Press, 1981. Ignore the word "Calculator" in the
    title; this is an essential reference if you're interested in
    developing software which calculates planetary positions, orbits,
    eclipses, and the like. If you're interested in pursuing such
    programming, you should also obtain:

    "Astronomical Formulae for Calculators" by Jean Meeus 1985. A must-have.

    "Planetary Programs and Tables from -4000 to +2800" by Pierre
    Bretagnon and Jean-Louis Simon, Willmann-Bell, 1986.

    This program was written by:

	John Walker
	http://www.fourmilab.ch/
       See LICENSE
*/

#include <math.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

extern double phase(double pdate, double *restrict pphase, double *restrict mage);
extern time_t date_parse(char *str);

#define halfmonth   14.76529434    /* Half Synodic month (new Moon to full Moon) */

#define PI 3.14159265358979323846  /* Assume not near black hole nor in Tennessee */

char *phasenames[]  = { "New", "Waxing Crescent", "First Quarter", "Waxing Gibbous", "Full", "Waning Gibbous", "Last Quarter", "Waning Crescent" };
char *emojis[]      = {"🌑", "🌒", "🌓", "🌔", "🌕",  "🌖", "🌗", "🌘"};
char *emojis_south[]= {"🌑", "🌘", "🌗", "🌖", "🌕",  "🌔", "🌓", "🌒"};

#define HELPTXT "mprintf [-h] [-t TIME] [-f FORMAT]"

static long jdate(struct tm *t);

/* JTIME --    Convert internal GMT date and time to astronomical Julian time (i.e. Julian date plus day fraction). double jtime (struct tm *t) */
#define jtime(t) ((jdate (t) - 0.5) + (t->tm_sec + 60 * (t->tm_min + 60 * t->tm_hour)) / 86400.0)

// JDATE  --  Convert internal GMT date and time to Julian day and fraction.
static long jdate (struct tm *t)
{
	long c, m, y = t->tm_year + 1900;

	m = t->tm_mon + 1;
	if (m > 2) m -= 3;
	else { m += 9; y--; }
	c = y / 100L;		   /* Compute century */
	y -= 100L * c;
	return (t->tm_mday + (c * 146097L) / 4 + (y * 1461L) / 4 + (m * 153L + 2) / 5 + 1721119L);
}

// PHASEINDEX --  Return phase based on Illuminated Fraction of the moon and age of the moon
static int phaseindex (double ilumfrac, double mage)
{
  if      (ilumfrac < 0.04) return 0;
  else if (ilumfrac > 0.96) return 4;
  else if (ilumfrac > 0.46 && ilumfrac < 0.54) return (mage > halfmonth) ? 6 : 2;
  else if (ilumfrac > 0.54 && ilumfrac < 0.96) return (mage > halfmonth) ? 5 : 3;
  else    return (mage > halfmonth) ? 7 : 1;
}

void mprintf(char *fmt, double ilumfrac, double mage, struct tm *time)
{
  // long then = microtime();
  int indx = phaseindex(ilumfrac, mage);
  for (size_t i = 0; i < strlen(fmt); i++) {
    if (fmt[i] != '%') putchar(fmt[i]);
    else switch (fmt[++i]) {
      case '\0':dprintf(2,"Error: Bad output formatting\n"); exit(1);
      case '%': putchar('%'); break;
      case 'n': putchar('\n'); break;
      case 't': putchar('\t'); break;
      case 'a': printf("%2.1f", mage); break;
      case 'J': printf("%f", jtime(time)); break;
      case 'e': fputs(emojis[indx], stdout); break;
      case 's': fputs(emojis_south[indx], stdout); break;
      case 'p': fputs(phasenames[indx], stdout); break;
      case 'P': printf("%2.1f", ilumfrac*100); break;
      case 'N': printf("%d", indx); break;
      default : dprintf(2, "Unknown flag"); break;
    }
  }
  putchar('\n');
  // printf("%ld\n", microtime()-then);
}

int main (int argc, char **argv)
{
  setvbuf(stdout, NULL, _IOFBF, 0);
  time_t now = time(0);
  char *fmtstr = "%p %e (%P%%)";

  //Option parsing
  for (int i = 0; (i = getopt (argc, argv, "ht:f:")) != -1; ) switch (i) {
    case 'h': puts(HELPTXT); exit(1);
    case 't': now = date_parse(optarg); break;
    case 'f': fmtstr = strdup(optarg); break;
    default: puts("Error: Unknown Option\n"HELPTXT); exit(1);
    }

  struct tm *gm = gmtime(&now);
  if (!gm) perror(argv[0]), exit(2);
  double cdtd = jtime(gm), ilfrac, moonage;
  phase (cdtd, &ilfrac, &moonage);

  mprintf(fmtstr, ilfrac, moonage, gm);
}

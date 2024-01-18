/* A Moon for the Unix Machine, Release 3.0

 Make with:
 cc -O2 moontool.c -o moontool -lm

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

    This  program is in the public domain: "Do what thou wilt shall be the
    whole of the law".  I'd appreciate  receiving  any  bug  fixes  and/or
    enhancements,  which  I'll  incorporate  in  future  versions  of  the
    program.  Please leave the original attribution information intact	so
    that credit and blame may be properly apportioned.
*/

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*  Astronomical constants  */
#define epoch	      2444238.5	     /* 1980 January 0.0 */
/*  Constants defining the Sun's apparent orbit  */
#define elonge	    278.833540	   /* Ecliptic longitude of the Sun at epoch 1980.0 */
#define elongp	    282.596403	   /* Ecliptic longitude of the Sun at perigee */
#define eccent      0.016718       /* Eccentricity of Earth's orbit */
#define sunsmax     1.495985e8     /* Semi-major axis of Earth's orbit, km */
#define sunangsiz   0.533128       /* Sun's angular size, degrees, at semi-major axis distance */
/*  Elements of the Moon's orbit, epoch 1980.0  */
#define mmlong      64.975464      /* Moon's mean lonigitude at the epoch */
#define mmlongp     349.383063	   /* Mean longitude of the perigee at the epoch */
#define mlnode	    151.950429	   /* Mean longitude of the node at the epoch */
#define minc        5.145396       /* Inclination of the Moon's orbit */
#define mecc        0.054900       /* Eccentricity of the Moon's orbit */
#define mangsiz     0.5181         /* Moon's angular size at distance a from Earth */
#define msmax       384401.0       /* Semi-major axis of Moon's orbit in km */
#define mparallax   0.9507	       /* Parallax at distance a from Earth */
#define synmonth    29.53058868    /* Synodic month (new Moon to new Moon) */
#define halfmonth   14.76529434    /* Half Synodic month (new Moon to full Moon) */
#define lunatbase   2423436.0      /* Base date for E. W. Brown's numbered series of lunations (1923 January 16) */
/*  Properties of the Earth  */
#define earthrad    6378.16	       /* Radius of Earth in kilometres */

#define PI 3.14159265358979323846  /* Assume not near black hole nor in Tennessee */

/*  Handy mathematical functions  */
#define sgn(x)      (((x) < 0) ? -1 : ((x) > 0 ? 1 : 0))	/* Extract sign */
#define fixangle(a) ((a) - 360.0 * (floor((a) / 360.0)))  /* Fix angle	  */
#define torad(d)    ((d) * (M_PI / 180.0))			          /* Deg->Rad	    */
#define todeg(d)    ((d) * (180.0 / M_PI))			          /* Rad->Deg	    */
#define dsin(x)     (sin(torad((x))))			                /* Sin from deg */
#define dcos(x)     (cos(torad((x))))			                /* Cos from deg */

#define resp *restrict
#define dpr double *restrict

char *phasenames[]  = { "New", "Waxing Crescent", "First Quarter", "Waxing Gibbous", "Full", "Waning Gibbous", "Last Quarter", "Waning Crescent" };
char *emojis[]      = {"🌑", "🌒", "🌓", "🌔", "🌕", "🌘", "🌗" ,"🌖"};
char *emojis_south[]= {"🌑", "🌘", "🌗", "🌖", "🌕", "🌒", "🌓" ,"🌔"};

/*  Forward functions  */
extern double phase();
void	        mprintf();
static long   jdate();
static int    phaseindex();

/* JTIME --    Convert internal GMT date and time to astronomical Julian time (i.e. Julian date plus day fraction). double jtime (struct tm *t) */
#define jtime(t) ((jdate (t) - 0.5) + (t->tm_sec + 60 * (t->tm_min + 60 * t->tm_hour)) / 86400.0)

#define HELPTXT "moontool [-h] [-t TIME] [-f FORMAT]"

int main (int argc, char **argv)
{
  time_t now = time(0);
  char *fmtstr = "%p: %e (%P%%)";

  //Option parsing
  for (int i = 0; (i = getopt (argc, argv, "ht:f:")) != -1; ) switch (i) {
    case 'h': puts(HELPTXT); exit(1);
    case 't': now = atoi(optarg); break;
    case 'f': fmtstr = strdup(optarg); break;
    default: puts("Error: Unknown Option\n"HELPTXT); exit(1);
    }

  struct tm *gm = gmtime(&now);
  double cdtd = jtime(gm), ilfrac, moonage, dist, angdia, sundist, sunangdia;
  phase (cdtd, &ilfrac, &moonage, &dist, &angdia, &sundist, &sunangdia);

  //int indx = phaseindex(ilfrac,mage);
  //printf("%s: %s (%2.1f%%)\n", phasenames[indx], emojis[indx], ilfrac*100);
  mprintf(fmtstr, ilfrac, moonage, gm);
}

// PHASEINDEX --  Return phase based on Illuminated Fraction of the moon and age of the moon
static int phaseindex (double ilumfrac, double mage)
{
  int indx;
  if (ilumfrac < 0.04) return 0;
  else if (ilumfrac > 0.96) return 4;
  else if (ilumfrac > 0.46 && ilumfrac < 0.54) return (mage > halfmonth) ? 6 : 2;
  else if (ilumfrac > 0.54) return (mage > halfmonth) ? 5 : 3;
  else return (mage > halfmonth) ? 7 : 1;
  return indx;
}

void mprintf(char *fmt, double ilumfrac, double mage, struct tm *time)
{
  int indx = phaseindex(ilumfrac, mage);
  for (int i = 0; i < strlen(fmt); i++) {
    if (fmt[i] != '%') putchar(fmt[i]);
    else switch (fmt[++i]) {
      case '\0': dprintf(2,"Error: Bad output formatting\n"); exit(1);
      case '%': putchar('%'); break;
      case 'a': printf("%2.1f", mage); break;
      case 'J': printf("%.1f", jtime(time)); break;
      case 'e': printf("%s", emojis[indx]); break;
      case 'n': putchar('\n'); break;
      case 'p': printf("%s", phasenames[indx]); break;
      case 'P': printf("%2.1f", ilumfrac*100); break;
      case 's': printf("%s", emojis_south[indx]); break;
      case 't': putchar('\t'); break;
      default: dprintf(2, "Unknown flag"); break;
    }
  }
  putchar('\n');
}

// JDATE  --  Convert internal GMT date and time to Julian day and fraction.
static long jdate (struct tm *t)
{
	long c, m, y;

	y = t->tm_year + 1900;
	m = t->tm_mon + 1;
	if (m > 2) m -= 3;
	else { m += 9; y--; }
	c = y / 100L;		   /* Compute century */
	y -= 100L * c;
	return (t->tm_mday + (c * 146097L) / 4 + (y * 1461L) / 4 + (m * 153L + 2) / 5 + 1721119L);
}


// JHMS  --  Convert Julian time to hour, minutes, and seconds.
static void jhms(double j, int resp h, int resp m, int resp s)
{
	long ij;

	j += 0.5;				/* Astronomical to civil */
	ij = (j - floor(j)) * 86400.0;
	*h = ij / 3600L;
	*m = (ij / 60L) % 60L;
	*s = ij % 60L;
}

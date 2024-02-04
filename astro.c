/* Adapted from "moontool.c" by John Walker, Release 2.5
**
** Quoting from the original:
**
**   The algorithms used in this program to calculate the positions Sun and
**   Moon as seen from the Earth are given in the book "Practical Astronomy
**   With  Your  Calculator"  by  Peter  Duffett-Smith,   Second   Edition,
**   Cambridge University Press, 1981.  Ignore the word "Calculator" in the
**   title;  this  is  an  essential  reference  if  you're  interested  in
**   developing  software  which  calculates  planetary  positions, orbits,
**   eclipses, and  the  like.   If  you're  interested  in  pursuing  such
**   programming, you should also obtain:
**
**     "Astronomical  Formulae for Calculators" by Jean Meeus, Third Edition,
**     Willmann-Bell, 1985.  A must-have.
**
**     "Planetary  Programs  and  Tables  from  -4000  to  +2800"  by  Pierre
**     Bretagnon  and Jean-Louis Simon, Willmann-Bell, 1986.  If you want the
**     utmost  (outside  of  JPL)  accuracy  for  the  planets,  it's   here.
**
**     "Celestial BASIC" by Eric Burgess, Revised Edition, Sybex, 1985.  Very
**     cookbook oriented, and many of the algorithms are hard to dig  out  of
**     the turgid BASIC code, but you'll probably want it anyway.
**
** See http://www.fourmilab.ch/moontool/
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "astro.h"

/*  Astronomical constants  */

#define epoch 2444238.5 /* 1980 January 0.0 */

/*  Constants defining the Sun's apparent orbit  */

#define elonge 278.833540 /* Ecliptic longitude of the Sun \
                             at epoch 1980.0 */
#define elongp 282.596403 /* Ecliptic longitude of the Sun at \
                             perigee */
#define eccent 0.016718 /* Eccentricity of Earth's orbit */

/*  Elements of the Moon's orbit, epoch 1980.0  */

#define mmlong 64.975464 /* Moon's mean lonigitude at the epoch */
#define mmlongp 349.383063 /* Mean longitude of the perigee at the \
                              epoch */
#define mlnode 151.950429 /* Mean longitude of the node at the \
                             epoch */
#define minc 5.145396 /* Inclination of the Moon's orbit */
#define synmonth 29.53058868 /* Synodic month (new Moon to new Moon) */

/*  Properties of the Earth  */

#define earthrad 6378.16 /* Radius of Earth in kilometres */

#define PI 3.14159265358979323846 /* Assume not near black hole nor in \
                                     Tennessee */

/*  Handy mathematical functions  */

#define sgn(x) (((x) < 0) ? -1 : ((x) > 0 ? 1 : 0)) /* Extract sign */
#define abs(x) ((x) < 0 ? (-(x)) : (x)) /* Absolute val */
#define fixangle(a) ((a)-360.0 * (floor((a) / 360.0))) /* Fix angle	  */
#define torad(d) ((d) * (M_PI / 180.0)) /* Deg->Rad	  */
#define todeg(d) ((d) * (180.0 / M_PI)) /* Rad->Deg	  */
#define dsin(x) (sin(torad((x)))) /* Sin from deg */
#define dcos(x) (cos(torad((x)))) /* Cos from deg */

/*
 * JYEAR  --  Convert Julian date to year, month, day, which are
 *	       returned via integer pointers to integers.
 */

static void
jyear(double td, int* yy, int* mm, int* dd)
{
  double j, d, y, m;

  td += 0.5; /* Astronomical to civil */
  j = floor(td);
  j = j - 1721119.0;
  y = floor(((4 * j) - 1) / 146097.0);
  j = (j * 4.0) - (1.0 + (146097.0 * y));
  d = floor(j / 4.0);
  j = floor(((4.0 * d) + 3.0) / 1461.0);
  d = ((4.0 * d) + 3.0) - (1461.0 * j);
  d = floor((d + 4.0) / 4.0);
  m = floor(((5.0 * d) - 3) / 153.0);
  d = (5.0 * d) - (3.0 + (153.0 * m));
  d = floor((d + 5.0) / 5.0);
  y = (100.0 * y) + j;
  if (m < 10.0)
    m = m + 3;
  else {
    m = m - 9;
    y = y + 1;
  }
  *yy = y;
  *mm = m;
  *dd = d;
}

/*
 * MEANPHASE  --  Calculates time of the mean new Moon for a given
 *		base date.  This argument K to this function is
 *		the precomputed synodic month index, given by:
 *
 *			K = (year - 1900) * 12.3685
 *
 *		where year is expressed as a year and fractional year.
 */
static double
meanphase(double sdate, double k)
{
  double t, t2, t3, nt1;

  /* Time in Julian centuries from 1900 January 0.5 */
  t = (sdate - 2415020.0) / 36525;
  t2 = t * t; /* Square for frequent use */
  t3 = t2 * t; /* Cube for frequent use */

  nt1 = 2415020.75933 + synmonth * k
      + 0.0001178 * t2
      - 0.000000155 * t3
      + 0.00033 * dsin(166.56 + 132.87 * t - 0.009173 * t2);

  return nt1;
}

/*
 * TRUEPHASE  --  Given a K value used to determine the
 *		mean phase of the new moon, and a phase
 *		selector (0.0, 0.25, 0.5, 0.75), obtain
 *		the true, corrected phase time.
 */
static double
truephase(double k, double pha)
{
  double t, t2, t3, pt, m, mprime, f;
  int apcor = 0;

  k += pha; /* Add phase to new moon time */
  t = k / 1236.85; /* Time in Julian centuries from
                      1900 January 0.5 */
  t2 = t * t; /* Square for frequent use */
  t3 = t2 * t; /* Cube for frequent use */
  pt = 2415020.75933 /* Mean time of phase */
      + synmonth * k
      + 0.0001178 * t2
      - 0.000000155 * t3
      + 0.00033 * dsin(166.56 + 132.87 * t - 0.009173 * t2);

  m = 359.2242 /* Sun's mean anomaly */
      + 29.10535608 * k
      - 0.0000333 * t2
      - 0.00000347 * t3;
  mprime = 306.0253 /* Moon's mean anomaly */
      + 385.81691806 * k
      + 0.0107306 * t2
      + 0.00001236 * t3;
  f = 21.2964 /* Moon's argument of latitude */
      + 390.67050646 * k
      - 0.0016528 * t2
      - 0.00000239 * t3;
  if ((pha < 0.01) || (abs(pha - 0.5) < 0.01)) {

    /* Corrections for New and Full Moon */

    pt += (0.1734 - 0.000393 * t) * dsin(m)
        + 0.0021 * dsin(2 * m)
        - 0.4068 * dsin(mprime)
        + 0.0161 * dsin(2 * mprime)
        - 0.0004 * dsin(3 * mprime)
        + 0.0104 * dsin(2 * f)
        - 0.0051 * dsin(m + mprime)
        - 0.0074 * dsin(m - mprime)
        + 0.0004 * dsin(2 * f + m)
        - 0.0004 * dsin(2 * f - m)
        - 0.0006 * dsin(2 * f + mprime)
        + 0.0010 * dsin(2 * f - mprime)
        + 0.0005 * dsin(m + 2 * mprime);
    apcor = 1;
  } else if ((abs(pha - 0.25) < 0.01 || (abs(pha - 0.75) < 0.01))) {
    pt += (0.1721 - 0.0004 * t) * dsin(m)
        + 0.0021 * dsin(2 * m)
        - 0.6280 * dsin(mprime)
        + 0.0089 * dsin(2 * mprime)
        - 0.0004 * dsin(3 * mprime)
        + 0.0079 * dsin(2 * f)
        - 0.0119 * dsin(m + mprime)
        - 0.0047 * dsin(m - mprime)
        + 0.0003 * dsin(2 * f + m)
        - 0.0004 * dsin(2 * f - m)
        - 0.0006 * dsin(2 * f + mprime)
        + 0.0021 * dsin(2 * f - mprime)
        + 0.0003 * dsin(m + 2 * mprime)
        + 0.0004 * dsin(m - 2 * mprime)
        - 0.0003 * dsin(2 * m + mprime);
    if (pha < 0.5)
      /* First quarter correction */
      pt += 0.0028 - 0.0004 * dcos(m) + 0.0003 * dcos(mprime);
    else
      /* Last quarter correction */
      pt += -0.0028 + 0.0004 * dcos(m) - 0.0003 * dcos(mprime);
    apcor = 1;
  }
  if (!apcor) {
    (void)fprintf(stderr,
        "TRUEPHASE called with invalid phase selector.\n");
    abort();
  }
  return pt;
}

/*
 * PHASEHUNT2  --  Find time of phases of the moon which surround
 *		the current date.  Two phases are found.
 */
void phasehunt2(double sdate, double phases[2], double which[2])
{
  double adate, k1, k2, nt1, nt2;
  int yy, mm, dd;

  adate = sdate - 45;

  jyear(adate, &yy, &mm, &dd);
  k1 = floor((yy + ((mm - 1) * (1.0 / 12.0)) - 1900) * 12.3685);

  adate = nt1 = meanphase(adate, k1);
  while (1) {
    adate += synmonth;
    k2 = k1 + 1;
    nt2 = meanphase(adate, k2);
    if (nt1 <= sdate && nt2 > sdate)
      break;
    nt1 = nt2;
    k1 = k2;
  }
  which[0] = 0.0;
  which[1] = 0.25;
  phases[0] = truephase(k1, 0.0);
  phases[1] = truephase(k1, 0.25);
  if (phases[1] <= sdate) {
    which[0] = which[1];
    which[1] = 0.5;
    phases[0] = phases[1];
    phases[1] = truephase(k1, 0.5);
    if (phases[1] <= sdate) {
      phases[0] = phases[1];
      phases[1] = truephase(k1, 0.75);
      which[0] = which[1];
      which[1] = 0.75;
      if (phases[1] <= sdate) {
        phases[0] = phases[1];
        phases[1] = truephase(k2, 0.0);
        which[0] = which[1];
        which[1] = 0.0;
      }
    }
  }
}

/*
 * KEPLER  --	Solve the equation of Kepler.
 */
static double
kepler(double m, double ecc)
{
  double e, delta;
#define EPSILON 1E-6

  e = m = torad(m);
  do {
    delta = e - ecc * sin(e) - m;
    e -= delta / (1 - ecc * cos(e));
  } while (abs(delta) > EPSILON);
  return e;
}

/*
 * PHASE  --  Calculate phase of moon as a fraction:
 *
 *	The argument is the time for which the phase is requested,
 *	expressed as a Julian date and fraction.  Returns the terminator
 *	phase angle as a percentage of a full circle (i.e., 0 to 1),
 *	and stores into pointer arguments the illuminated fraction of
 *      the Moon's disc, the Moon's age in days and fraction, the
 *	distance of the Moon from the centre of the Earth, and the
 *	angular diameter subtended by the Moon as seen by an observer
 *	at the centre of the Earth.
 *
 * pphase:		Illuminated fraction
 * mage:		Age of moon in days
 */
double phase(double pdate, double* pphase, double* mage)
{

  double Day, N, M, Ec, Lambdasun, ml, MM, Ev, Ae, A3, MmP,
      mEc, A4, lP, V, lPP, MoonAge, MoonPhase;

  /* Calculation of the Sun's position */

  Day = pdate - epoch; /* Date within epoch */
  N = fixangle((360 / 365.2422) * Day); /* Mean anomaly of the Sun */
  M = fixangle(N + elonge - elongp); /* Convert from perigee
                                     co-ordinates to epoch 1980.0 */
  Ec = kepler(M, eccent); /* Solve equation of Kepler */
  Ec = sqrt((1 + eccent) / (1 - eccent)) * tan(Ec / 2);
  Ec = 2 * todeg(atan(Ec)); /* True anomaly */
  Lambdasun = fixangle(Ec + elongp); /* Sun's geocentric ecliptic
                                             longitude */
  /* Orbital distance factor */

  /* Calculation of the Moon's position */

  /* Moon's mean longitude */
  ml = fixangle(13.1763966 * Day + mmlong);

  /* Moon's mean anomaly */
  MM = fixangle(ml - 0.1114041 * Day - mmlongp);

  /* Evection */
  Ev = 1.2739 * sin(torad(2 * (ml - Lambdasun) - MM));

  /* Annual equation */
  Ae = 0.1858 * sin(torad(M));

  /* Correction term */
  A3 = 0.37 * sin(torad(M));

  /* Corrected anomaly */
  MmP = MM + Ev - Ae - A3;

  /* Correction for the equation of the centre */
  mEc = 6.2886 * sin(torad(MmP));

  /* Another correction term */
  A4 = 0.214 * sin(torad(2 * MmP));

  /* Corrected longitude */
  lP = ml + Ev + mEc - Ae + A4;

  /* Variation */
  V = 0.6583 * sin(torad(2 * (lP - Lambdasun)));

  /* True longitude */
  lPP = lP + V;

  /* Age of the Moon in degrees */
  MoonAge = lPP - Lambdasun;

  /* Phase of the Moon */
  MoonPhase = (1 - cos(torad(MoonAge))) / 2;

  *pphase = MoonPhase;
  *mage = synmonth * (fixangle(MoonAge) / 360.0);
  return fixangle(MoonAge) / 360.0;
}

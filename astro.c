/* A Moon for the Unix Machine, Release 3.0

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

#include <math.h>

/*  Astronomical constants  */
/*  Constants defining the Sun's apparent orbit  */
#define elongp	    282.596403	   /* Ecliptic longitude of the Sun at perigee */
#define eccent      0.016718       /* Eccentricity of Earth's orbit */

#define PI 3.14159265358979323846  /* Assume not near black hole nor in Tennessee */

/*  Handy mathematical functions  */
#define fixangle(a) ((a) - 360.0 * (floor((a) / 360.0)))  /* Fix angle	  */
#define torad(d)    ((d) * (M_PI / 180.0))			          /* Deg->Rad	    */
#define todeg(d)    ((d) * (180.0 / M_PI))			          /* Rad->Deg	    */

#define resp *restrict
#define dpr double *restrict

// KEPLER  --	Solve the equation of Kepler.
static double kepler(double m, double ecc)
{
	double e, delta;

	e = m = torad(m);
	do {
		delta = e - ecc * sin(e) - m;
		e -= delta / (1 - ecc * cos(e));
	} while (fabs (delta) > 1E-6); // 1E-6 Epsilon
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
 */

double phase (double pdate, dpr pphase, dpr mage)
  /*pdate     Julian date of phase */
  /*pphase    Illuminated fraction */
  /*mage      Age of moon in days */
{
	double	Day, M, Ec, Lambdasun, ml, MM, Ev, Ae, MmP, lP, MoonAge;

  /* Calculation of the Sun's position */
	Day = pdate - 2444238.5;			/* Date within epoch (1/1/1980) */
	M = fixangle(fixangle((360 / 365.2422) * Day) + 278.833540 - elongp);	/* Convert from perigee co-ordinates to epoch 1980.0 */
	Ec = kepler(M, eccent); 		/* Solve equation of Kepler */
	Ec = sqrt((1 + eccent) / (1 - eccent)) * tan(Ec / 2);
	Ec = 2 * todeg(atan(Ec));		/* True anomaly */
  Lambdasun = fixangle(Ec + elongp);      /* Sun's geocentric ecliptic longitude */

  /* Calculation of the Moon's position */

  /* Moon's mean longitude */
	ml = fixangle(13.1763966 * Day + 64.975464); // 64.975464: Moons Mean longitude at epoch

  /* Moon's mean anomaly */
	MM = fixangle(ml - 0.1114041 * Day - 349.383063); /* Mean longitude of the perigee at the epoch */

	/* Evection */
	Ev = 1.2739 * sin(torad(2 * (ml - Lambdasun) - MM));

	/* Annual equation */
	Ae = 0.1858 * sin(torad(M));

	/* Corrected anomaly */
	MmP = MM + Ev - Ae - (0.37 * sin(torad(M)));

	/* Corrected longitude */
	lP = ml + Ev + (6.2886 * sin(torad(MmP))) - Ae + (0.214 * sin(torad(2 * MmP)));

	/* Age of the Moon in degrees */
	MoonAge = (lP + (0.6583 * sin(torad(2 * (lP - Lambdasun))))) - Lambdasun;

	*pphase = (1 - cos(torad(MoonAge))) / 2;
	*mage = 29.53058868 * (fixangle(MoonAge) / 360.0); // 29.53058868: Synodic Month Length
	return fixangle(MoonAge) / 360.0;
}

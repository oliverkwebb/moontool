/* A moon for the unix machine, 3.0. See bottom comment */

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

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
#define lunatbase   2423436.0      /* Base date for E. W. Brown's numbered series of lunations (1923 January 16) */

/*  Properties of the Earth  */
#define earthrad    6378.16	       /* Radius of Earth in kilometres */


#define PI M_PI //3.14159265358979323846  /* Assume not near black hole nor in Tennessee */ /*Note to self: Put that in fortune cookie file*/

/*  Handy mathematical functions  */
#define sgn(x)      (((x) < 0) ? -1 : ((x) > 0 ? 1 : 0))	/* Extract sign */
#define fixangle(a) ((a) - 360.0 * (floor((a) / 360.0)))  /* Fix angle	  */
#define torad(d)    ((d) * (PI / 180.0))			            /* Deg->Rad	    */
#define todeg(d)    ((d) * (180.0 / PI))			            /* Rad->Deg	    */
#define dsin(x)     (sin(torad((x))))			                /* Sin from deg */
#define dcos(x)     (cos(torad((x))))			                /* Cos from deg */

static char	*moname [] = {
                        "January", "February", "March", "April", "May",
                        "June", "July", "August", "September",
                        "October", "November", "December"
};

static char *emojis[] = {"🌕", "🌔" ,"🌓" ,"🌒" ,"🌑" ,"🌘" ,"🌗" ,"🌖"} ;
static char *phasenames[] = {
  "New", "Waxing Crescent",
  "First Quarter", "Waxing Gibbous",
  "Full", "Waning Gibbous", 
  "Last Quarter", "Waning Crescent"
};


/*  Forward functions  */

/* JTIME --    Convert internal GMT date and time to astronomical Julian time (i.e. Julian date plus day fraction, expressed as a double).
 static double jtime (struct tm *t) */

static long   jdate(struct tm* t);
#define jtime(t) ((jdate (t) - 0.5) + (t->tm_sec + 60 * (t->tm_min + 60 * t->tm_hour)) / 86400.0)

static double phase (double pdate, double *pphase, double *mage, double *dist, double *angdia, double *sudist, double *suangdia);
static void		phasehunt();
static void		jyear(), jhms();


int main (int argc, char **argv)
{
	/*int	c;

    while ((c = getopt (argc, argv, "cmbt")) != EOF) {
		switch (c) {
      case 't':
			testmode = 1;
			break;
      case 'c':
			color_mode = 1;
			break;
      case 'm':
			color_mode = 0;
			break;
      case 'b':
			border = 1;
			break;
		}
	}*/

  time_t asdc = (argc == 1 ? time(0) : atoi(argv[1])) ;
  struct tm *gm = gmtime(&asdc);
  double cdtd = jtime(gm), ppas, u1, u2, u3, u4, u5;
  phase (cdtd, &ppas, &u1, &u2, &u3, &u4, &u5);
  int indx = 0;

  if (ppas < 0.04) indx = 0;
  else if (ppas > 0.96) indx = 4;
  else if (ppas > 0.46 && ppas < 0.54) {
    indx = 2;
    if (u1 > 14) indx +=4; 
  } else if (ppas > 0.54) {
    indx = 3;
    if (u1 > 14) indx +=2; 
  } else {
    indx = 1;
    if (u1 > 14) indx +=6; 
  }

  printf("%s: %s (%2.1f%%)\n", phasenames[indx], emojis[indx], ppas*100);
}

// FMT_PHASE_TIME -- Format the provided julian date into the provided buffer in UTC format for screen display
void fmt_phase_time (double utime, char *buf)
{
	int	yy, mm, dd, hh, mmm, ss;

	jyear (utime, &yy, &mm, &dd);
	jhms (utime, &hh, &mmm, &ss);
  printf (buf, "%2d:%02d UTC %2d %s %d            ", hh, mmm, dd, moname [mm - 1], yy);
}

// JDATE  --  Convert internal GMT date and time to Julian day and fraction.
static long jdate (struct tm *t)
{
	long c, m, y;

	y = t->tm_year + 1900;
	m = t->tm_mon + 1;
	if (m > 2) {
		m = m - 3;
	} else {
		m = m + 9;
		y--;
	}
	c = y / 100L;		   /* Compute century */
	y -= 100L * c;
	return (t->tm_mday + (c * 146097L) / 4 + (y * 1461L) / 4 + (m * 153L + 2) / 5 + 1721119L);
}

// JYEAR  --  Convert Julian date to year, month, day, which are returned via integer pointers to integers.
static void jyear (double td, int *yy, int *mm, int *dd)
{
	double j, d, y, m;

	td += 0.5;				/* Astronomical to civil */
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
	if (m < 10.0) m+=3;
	else { m-=9; y++; }
	*yy = y;
	*mm = m;
	*dd = d;
}


// JHMS  --  Convert Julian time to hour, minutes, and seconds.

static void jhms(double j, int *h, int *m, int *s)
{
	long ij;

	j += 0.5;				/* Astronomical to civil */
	ij = (j - floor(j)) * 86400.0;
	*h = ij / 3600L;
	*m = (ij / 60L) % 60L;
	*s = ij % 60L;
}


/*
 * MEANPHASE  --  Calculates time of the mean new Moon for a given
 * base date.  This argument K to this function is the precomputed synodic month index, given by:
 *			K = (year - 1900) * 12.3685
 *	where year is expressed as a year and fractional year.
 */

static double meanphase (double sdate, double k)
{
	double	t, t2, t3, nt1;

	/* Time in Julian centuries from 1900 January 0.5 */
	t = (sdate - 2415020.0) / 36525;
	t2 = t * t;			/* Square for frequent use */
	t3 = t2 * t;			/* Cube for frequent use */

	nt1 = 2415020.75933 + synmonth * k
		+ 0.0001178 * t2
		- 0.000000155 * t3
		+ 0.00033 * dsin(166.56 + 132.87 * t - 0.009173 * t2);

	return nt1;
}


/*
 * 1PHASE  --  Given a K value used to determine the
 *		mean phase of the new moon, and a phase
 *		selector (0.0, 0.25, 0.5, 0.75), obtain
 *		the true, corrected phase time.
 */

static double truephase(double k, double phase)
{
	double t, t2, t3, pt, m, mprime, f;
	int apcor = 0;

	k += phase;		   /* Add phase to new moon time */
	t = k / 1236.85;	   /* Time in Julian centuries from
				      1900 January 0.5 */
	t2 = t * t;		   /* Square for frequent use */
	t3 = t2 * t;		   /* Cube for frequent use */
	pt = 2415020.75933	   /* Mean time of phase */
	     + synmonth * k
	     + 0.0001178 * t2
	     - 0.000000155 * t3
	     + 0.00033 * dsin(166.56 + 132.87 * t - 0.009173 * t2);

        m = 359.2242               /* Sun's mean anomaly */
	    + 29.10535608 * k
	    - 0.0000333 * t2
	    - 0.00000347 * t3;
        mprime = 306.0253          /* Moon's mean anomaly */
	    + 385.81691806 * k
	    + 0.0107306 * t2
	    + 0.00001236 * t3;
        f = 21.2964                /* Moon's argument of latitude */
	    + 390.67050646 * k
	    - 0.0016528 * t2
	    - 0.00000239 * t3;
	if ((phase < 0.01) || (fabs(phase - 0.5) < 0.01)) {

	   /* Corrections for New and Full Moon */

	   pt +=     (0.1734 - 0.000393 * t) * dsin(m)
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
	} else if ((fabs(phase - 0.25) < 0.01 || (fabs(phase - 0.75) < 0.01))) {
	   pt +=     (0.1721 - 0.0004 * t) * dsin(m)
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
	   if (phase < 0.5)
	      /* First quarter correction */
	      pt += 0.0028 - 0.0004 * dcos(m) + 0.0003 * dcos(mprime);
	   else
	      /* Last quarter correction */
	      pt += -0.0028 + 0.0004 * dcos(m) - 0.0003 * dcos(mprime);
	   apcor = 1;
	}
	if (!apcor) {
	   (void)fprintf (stderr,
                "1PHASE called with invalid phase selector.\n");
	   abort();
	}
	return pt;
}


// PHASEHUNT  --  Find time of phases of the moon which surround the
// current date.  Five phases are found, starting and ending with the new
// moons which bound the current lunation.

static void phasehunt (double sdate, double phases[5])
{
	double	adate, k1, k2, nt1, nt2;
	int	yy, mm, dd;

	adate = sdate - 45;

	jyear(adate, &yy, &mm, &dd);
	k1 = floor((yy + ((mm - 1) * (1.0 / 12.0)) - 1900) * 12.3685);

	adate = nt1 = meanphase(adate, k1);
	while (1) {
		adate += synmonth;
		k2 = k1 + 1;
		nt2 = meanphase(adate, k2);
		if (nt1 <= sdate && nt2 > sdate) break;
		nt1 = nt2;
		k1 = k2;
	}
	phases [0] = truephase (k1, 0.0);
	phases [1] = truephase (k1, 0.25);
	phases [2] = truephase (k1, 0.5);
	phases [3] = truephase (k1, 0.75);
	phases [4] = truephase (k2, 0.0);
}


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

static double phase (double pdate, double *pphase, double *mage, double *dist, double *angdia, double *sudist, double *suangdia)
  /*pdate     Julian date of phase */
  /*pphase    Illuminated fraction */
  /*mage      Age of moon in days */
  /*dist      Distance in kilometres */
  /*angdia    Angular diameter in degrees */
  /*sudist    Distance to Sun */
  /*suangdia  Sun's angular diameter */
{

	double	Day, N, M, Ec, Lambdasun, ml, MM, MN, Ev, Ae, A3, MmP,
		mEc, A4, lP, V, lPP, NP, y, x, Lambdamoon, MoonAge, MoonPhase,
		MoonDist, MoonDFrac, MoonAng, F, SunDist, SunAng;

        /* Calculation of the Sun's position */

	Day = pdate - epoch;			/* Date within epoch */
	N = fixangle((360 / 365.2422) * Day);	/* Mean anomaly of the Sun */
	M = fixangle(N + elonge - elongp);	/* Convert from perigee
						co-ordinates to epoch 1980.0 */
	Ec = kepler(M, eccent); 		/* Solve equation of Kepler */
	Ec = sqrt((1 + eccent) / (1 - eccent)) * tan(Ec / 2);
	Ec = 2 * todeg(atan(Ec));		/* True anomaly */
        Lambdasun = fixangle(Ec + elongp);      /* Sun's geocentric ecliptic
							longitude */
	/* Orbital distance factor */
	F = ((1 + eccent * cos(torad(Ec))) / (1 - eccent * eccent));
	SunDist = sunsmax / F;			/* Distance to Sun in km */
        SunAng = F * sunangsiz;         /* Sun's angular size in degrees */


        /* Calculation of the Moon's position */

        /* Moon's mean longitude */
	ml = fixangle(13.1763966 * Day + mmlong);

        /* Moon's mean anomaly */
	MM = fixangle(ml - 0.1114041 * Day - mmlongp);

        /* Moon's ascending node mean longitude */
	MN = fixangle(mlnode - 0.0529539 * Day);

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

	/* Corrected longitude of the node */
	NP = MN - 0.16 * sin(torad(M));

	/* Y inclination coordinate */
	y = sin(torad(lPP - NP)) * cos(torad(minc));

	/* X inclination coordinate */
	x = cos(torad(lPP - NP));

	/* Ecliptic longitude */
	Lambdamoon = todeg(atan2(y, x));
	Lambdamoon += NP;

	/* Calculation of the phase of the Moon */

	/* Age of the Moon in degrees */
	MoonAge = lPP - Lambdasun;

	/* Phase of the Moon */
	MoonPhase = (1 - cos(torad(MoonAge))) / 2;

	/* Calculate distance of moon from the centre of the Earth */

	MoonDist = (msmax * (1 - mecc * mecc)) /
	   (1 + mecc * cos(torad(MmP + mEc)));

        /* Calculate Moon's angular diameter */

	MoonDFrac = MoonDist / msmax;
	MoonAng = mangsiz / MoonDFrac;

        /* Calculate Moon's parallax */

	*pphase = MoonPhase;
	*mage = synmonth * (fixangle(MoonAge) / 360.0);
	*dist = MoonDist;
	*angdia = MoonAng;
	*sudist = SunDist;
	*suangdia = SunAng;
	return fixangle(MoonAge) / 360.0;
}

/*  A Moon for the Unix Machine, Release 3.0

    Designed and implemented by John Walker in December 1987,
    Revised and updated in February of 1988.
    Revised and updated again in June of 1988 by Ron Hitchens.
    Revised and updated yet again in July/August of 1989 by Ron Hitchens.
    Revised and updated yet yet again in January of 2024 by Oliver Webb.

    Make with:
    cc -O2 moontool.c -o moontool -lm

    This is a program which displays the  current phase of the Moon.

    The algorithms used in this program to calculate the positions Sun and
    Moon as seen from the Earth are given in the book "Practical Astronomy
    With  Your  Calculator"  by  Peter  Duffett-Smith,   Second   Edition,
    Cambridge University Press, 1981.  Ignore the word "Calculator" in the
    title;  this  is  an  essential  reference  if  you're  interested  in
    developing	software  which  calculates  planetary	positions, orbits,
    eclipses, and  the  like.   If  you're  interested  in  pursuing  such
    programming, you should also obtain:

    "Astronomical Formulae for Calculators" by Jean Meeus 1985.  A must-have.

    "Planetary  Programs  and  Tables  from  -4000  to  +2800"  by  Pierre
    Bretagnon  and Jean-Louis Simon, Willmann-Bell, 1986.

    This program was written by:

	John Walker
	http://www.fourmilab.ch/

    This  program is in the public domain: "Do what thou wilt shall be the
    whole of the law".  I'd appreciate  receiving  any  bug  fixes  and/or
    enhancements,  which  I'll  incorporate  in  future  versions  of  the
    program.  Please leave the original attribution information intact	so
    that credit and blame may be properly apportioned.

----------------
	History:
	--------
	June 1988	Version 2.0 posted to usenet by John Walker

	June 1988	Modified by Ron Hitchens to produce version 2.1
			modified icon generation to show surface texture
			on visible moon face.
  Eliminated "illegal" direct modification of icon image memory.
 added a menu to allow switching in and out of test mode, for
 entertainment value mostly.  reworked timer behaviour so that process
 doesn't wake up unnecessarily.  trap sigwinch signals to notice more
 easily when the tool opens and closes.  modified layout of information
 in open window display to reduce the amount of pixels modified in
 each update.
  Batched pixwin updates so that only one screen rasterop per cycle
  is done.
 changed open window to display white-on-black for a more aesthetic look,
 and to suggest the effect of looking at the moon in the nighttime sky.
 setup default tool and canvas colors to be the same as B&W monochrome,
 for those us lucky enough to have color monitors and who have the
 default monochrome colors set to something other than B&W (I like green
 on black myself) various code reformatting and pretty-printing to suit
 my own coding taste (I got a bit carried away).  code tweaking to make
 lint happy.  returned my hacked version to John.  (but he never got it)

	July 1989	Modified further for color displays.  On a color Sun,
			 four colors will be used for the canvas and the icon.
			 Rather than just show the illuminated portion of
			 the moon, a color icon will also show the darkened
			 portion in a dark blue shade.	The text on the icon
                         will also be drawn in a nice "buff" color, since there
			 was one more color left to use.
                        Add two command line args, "-c" and "-m" to explicitly
			 specify color or monochrome mode.  If neither are
                         given, moontool will try to determine if it's
			 running on a color or mono display by looking at the
                         depth of the frame's pixwin's pixrect.  This is not
			 always reliable on a Prism-type framebuffer like my
			 3/60C, so these two args will force one or the
			 other mode.
			Use getopt to parse the args.
			Change the tool menu slightly to use only one item
			 for switching in and out of test mode.
			A little more lint picking.

	July 1989	Modified a little bit more a few days later to use 8
			 colors and an accurate grey-scale moon face created
			 by Joe Hitchens on an Amiga.
			Added the -b option to draw a one pixel border around
			 the icon.  I like it, but it may not mesh well with
			 some backgrounds or adjacent icons.
			Added The Apollo 11 Commemorative Red Dot, to show
			 where Neil and Buzz went on vacation a few years ago.
			Updated man page.

        August 1989     Received version 2.3 of John Walker's original code.
			 Rolled in bug fixes to astronomical algorithms:

                         2.1  6/16/88   Bug fix.  Table of phases didn't update
					at the moment of the new moon.	Call on phasehunt didn't convert civil Julian
					date to astronomical Julian date.
					Reported by Dag Bruck
					 (dag@control.lth.se).

			 2.2  N/A	(superseded by my code)

			 2.3  6/7/89	Bug fix.  Table of phases skipped the
					phases for July 1989.  This occurred
					due to sloppy maintenance of the
					synodic month index in the interchange
					of information between phasehunt() and
					meanphase().  I simplified and
					corrected the handling of the month
					index as phasehunt() steps along and
					removed unneeded code from meanphase().
					Reported by Bill Randle of Tektronix.
					 (billr@saab.CNA.TEK.COM).

                        I've taken it upon myself to call this version 2.4

	Ron Hitchens
		ronbo@vixen.uucp
		...!uunet!cs.utah.edu!caeco!vixen!ronbo
		hitchens@cs.utexas.edu

	December 1999	Minor Y2K fix.	Release 2.5.

  January 2024:
    Throwing out anything that isn't astromony


*/

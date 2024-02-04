#ifndef _ASTRO_H_
#define _ASTRO_H_

/*
 * PHASEHUNT2  --  Find time of phases of the moon which surround
 *		the current date.  Two phases are found.
 */
void phasehunt2(double sdate, double phases[2], double which[2]);

/*
 * PHASE  --  Calculate phase of moon as a fraction:
 *
 *	The argument is the time for which the phase is requested,
 *	expressed as a Julian date and fraction.  Returns the terminator
 *	phase angle as a percentage of a full circle (i.e., 0 to 1),
 *	and stores into pointer arguments the illuminated fraction of
 *      the Moon's disc, the Moon's age in days.
 *
 * pphase:		Illuminated fraction
 * mage:		Age of moon in days
 */
double phase(double pdate, double *pphase, double *mage);

#endif /* _ASTRO_H_ */

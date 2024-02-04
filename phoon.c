/* phoon - show the phase of the moon
**
** ver  date   who remarks
** --- ------- --- -------------------------------------------------------------
** 04A 02feb24 OW  Revision of the source code to be c99/POSIX2006
** 03A 01apr95 JP  Updated to use date_parse.
** 02A 07jun88 JP  Changed the phase calculation to use code from John Walker's
**                   "moontool", increasing accuracy tremendously.
**                 Got rid of SINFORCOS, since Apple has probably fixed A/UX
**                   by now.
** 01I 03feb88 JP  Added 32 lines.
** 01H 14jan88 JP  Added 22 lines.
** 01G 05dec87 JP  Added random sabotage to generate Hubert.
**		   Defeated text stuff for moons 28 or larger.
** 01F 13oct87 JP  Added pumpkin19 in October.  Added hubert29.
** 01E 14may87 JP  Added #ifdef SINFORCOS to handle broken A/UX library.
** 01D 02apr87 JP  Added 21 lines.
** 01C 26jan87 JP  Added backgrounds for 29 and 18 lines.
** 01B 28dec86 JP  Added -l flag, and backgrounds for 19 and 24 lines.
** 01A 08nov86 JP  Translated from the ratfor version of 12nov85, which itself
**                   was translated from the Pascal version of 05apr79.
**
** Copyright (C) 1986,1987,1988,1995 by Jef Poskanzer <jef@mail.acme.com>.
** All rights reserved.
**
** See LICENSE
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

extern time_t date_parse(char* str);
#include "astro.h"

#define unix_to_julian(t) ((double)t / 86400.0 + 2440587.4999996666666666666)

/* If you change the aspect ratio, the canned backgrounds won't work. */
#define ASPECTRATIO 0.5

static void
putseconds(long secs)
{
  long days, hours, minutes;

  days = secs / 86400;
  secs -= days * 86400;
  hours = secs / 3600;
  secs -= hours * 3600;
  minutes = secs / 60;
  secs -= minutes * 60;

  printf("%ld %2ld:%02ld:%02ld", days, hours, minutes, secs);
}

static void putmoon(time_t t, int numlines)
{
  static char bg[23][47] = {
    "                 .------------.                ",
    "             .--'  o     . .   `--.            ",
    "          .-'   .    O   .       . `-.         ",
    "       .-'@   @@@@@@@   .  @@@@@      `-.      ",
    "      /@@@  @@@@@@@@@@@   @@@@@@@   .    \\     ",
    "    ./    o @@@@@@@@@@@   @@@@@@@       . \\.   ",
    "   /@@  o   @@@@@@@@@@@.   @@@@@@@   O      \\  ",
    "  /@@@@   .   @@@@@@@o    @@@@@@@@@@     @@@ \\ ",
    "  |@@@@@               . @@@@@@@@@@@@@ o @@@@| ",
    " /@@@@@  O  `.-./  .      @@@@@@@@@@@@    @@  \\",
    " | @@@@    --`-'       o     @@@@@@@@ @@@@    |",
    " |@ @@@        `    o      .  @@   . @@@@@@@  |",
    " |       @@  @         .-.     @@@   @@@@@@@  |",
    " \\  . @        @@@     `-'   . @@@@   @@@@  o /",
    "  |      @@   @@@@@ .           @@   .       | ",
    "  \\     @@@@  @\\@@    /  .  O    .     o   . / ",
    "   \\  o  @@     \\ \\  /         .    .       /  ",
    "    `\\     .    .\\.-.___   .      .   .-. /'   ",
    "      \\           `-'                `-' /     ",
    "       `-.   o   / |     o    O   .   .-'      ",
    "          `-.   /     .       .    .-'         ",
    "             `--.       .      .--'            ",
    "                 `------------'                "
  };
  static char qlits[4][16] = {
    "New Moon +     ",
    "First Quarter +",
    "Full Moon +    ",
    "Last Quarter + ",
  };
  static char nqlits[4][16] = {
    "New Moon -     ",
    "First Quarter -",
    "Full Moon -    ",
    "Last Quarter - ",
  };

  double jd, pctphase, angphase, cphase, aom;
  double phases[2], which[2];
  int lin, col, midlin;
  double mcap, yrad, xrad, y, xright, xleft;
  int colright, colleft;

  /* Figure out the phase. */
  jd = unix_to_julian(t);
  pctphase = phase(jd, &cphase, &aom);
  angphase = pctphase * 2.0 * M_PI;
  mcap = -cos(angphase);

  /* Figure out how big the moon is. */
  yrad = numlines / 2.0;
  xrad = yrad / ASPECTRATIO;

  /* Figure out some other random stuff. */
  midlin = numlines / 2;
  phasehunt2(jd, phases, which);

  /* Now output the moon, a slice at a time. */
  for (lin = 0; lin < numlines; lin = lin + 1) {
    /* Compute the edges of this slice. */
    y = lin + 0.5 - yrad;
    xright = xrad * sqrt(1.0 - (y * y) / (yrad * yrad));
    xleft = -xright;
    if (angphase >= 0.0 && angphase < M_PI)
      xleft = mcap * xleft;
    else
      xright = mcap * xright;
    colleft = (int)(xrad + 0.5) + (int)(xleft + 0.5);
    colright = (int)(xrad + 0.5) + (int)(xright + 0.5);

    /* Now output the slice. */
    for (col = 0; col < colleft; ++col)
      putchar(' ');
    for (; col <= colright; ++col)
      putchar(bg[lin][col]);
    /* Output the end-of-line information, if any. */
    if (lin == midlin - 2) {
      fputs("\t ", stdout);
      printf("%-16s", qlits[(int)(which[0] * 4.0 + 0.001)]);
    } else if (lin == midlin - 1) {
        fputs("\t ", stdout);
        putseconds((int)((jd - phases[0]) * 86400));
      } else if (lin == midlin) {
        fputs("\t ", stdout);
        printf("%-16s", nqlits[(int)(which[0] * 4.0 + 0.001)]);
      } else if (lin == midlin + 1) {
        fputs("\t ", stdout);
        putseconds((int)((phases[1] - jd) * 86400));
      }

    putchar('\n');
  }
}

/* Main program. */

int main(int argc, char** argv)
{
  time_t t;
  int numlines;
  char* usage = "usage: %s [<date/time>]\n";

  /* Parge args. */
  numlines = 23;

  /* Figure out what date and time to use. */
  if (!(argc - 1)) {
    /* No arguments present - use the current date and time. */
    t = time(0);
  } else if (argc - 1 == 1) {
    t = date_parse(argv[1]);
  } else {
    /* Too many args! */
    fprintf(stderr, usage, argv[0]);
    exit(1);
  }

  putmoon(t, numlines);

  /* All done. */
  exit(0);
}

/* phoon - show the phase of the moon
**
** ver  date   who remarks
** --- ------- --- -------------------------------------------------------------
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
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
**
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
** FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
** DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
** OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
** HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
** LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
** OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
** SUCH DAMAGE.
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "astro.h"
time_t date_parse(char *str);

/* Global defines and declarations. */

#define SECSPERMINUTE 60
#define SECSPERHOUR (60 * SECSPERMINUTE)
#define SECSPERDAY (24 * SECSPERHOUR)

#define PI 3.1415926535897932384626433

#define DEFAULTNUMLINES 23

#define QUARTERLITLEN 16
#define QUARTERLITLENPLUSONE 17

/* If you change the aspect ratio, the canned backgrounds won't work. */
#define ASPECTRATIO 0.5

static long jdate(struct tm *t);

/* JTIME --    Convert internal GMT date and time to astronomical Julian time (i.e. Julian date plus day fraction). double jtime (struct tm *t) */
#define jtime(t) ((jdate (t) - 0.5) + (t->tm_sec + 60 * (t->tm_min + 60 * t->tm_hour)) / 86400.0)

// JDATE  --  Convert internal GMT date and time to Julian day and fraction.
static long jdate(struct tm *t)
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

static void putseconds(long secs) {
  long days, hours, minutes;

  days = secs / SECSPERDAY;
  secs = secs - days * SECSPERDAY;
  hours = secs / SECSPERHOUR;
  secs = secs - hours * SECSPERHOUR;
  minutes = secs / SECSPERMINUTE;
  secs = secs - minutes * SECSPERMINUTE;

  printf("%ld %2ld:%02ld:%02ld", days, hours, minutes, secs);
}

static void putmoon(time_t t, int numlines) {
  static char background18[18][37] = {"             .----------.            ",
                                      "         .--'   o    .   `--.        ",
                                      "       .'@  @@@@@@ O   .   . `.      ",
                                      "     .'@@  @@@@@@@@   @@@@   . `.    ",
                                      "   .'    . @@@@@@@@  @@@@@@    . `.  ",
                                      "  / @@ o    @@@@@@.   @@@@    O   @\\ ",
                                      "  |@@@@               @@@@@@     @@| ",
                                      " / @@@@@   `.-.    . @@@@@@@@  .  @@\\",
                                      " | @@@@   --`-'  .  o  @@@@@@@      |",
                                      " |@ @@                 @@@@@@ @@@   |",
                                      " \\      @@    @   . ()  @@   @@@@@  /",
                                      "  |   @      @@@         @@@  @@@  | ",
                                      "  \\  .   @@  @\\  .      .  @@    o / ",
                                      "   `.   @@@@  _\\ /     .      o  .'  ",
                                      "     `.  @@    ()---           .'    ",
                                      "       `.     / |  .    o    .'      ",
                                      "         `--./   .       .--'        ",
                                      "             `----------'            "};
  static char background19[19][39] = {
      "              .----------.             ",
      "          .--'   o    .   `--.         ",
      "       .-'@  @@@@@@ O   .   . `-.      ",
      "     .' @@  @@@@@@@@   @@@@   .  `.    ",
      "    /     . @@@@@@@@  @@@@@@     . \\   ",
      "   /@@  o    @@@@@@.   @@@@    O   @\\  ",
      "  /@@@@                @@@@@@     @@@\\ ",
      " . @@@@@   `.-./    . @@@@@@@@  .  @@ .",
      " | @@@@   --`-'  .      @@@@@@@       |",
      " |@ @@        `      o  @@@@@@ @@@@   |",
      " |      @@        o      @@   @@@@@@  |",
      " ` .  @       @@     ()   @@@  @@@@   '",
      "  \\     @@   @@@@        . @@   .  o / ",
      "   \\   @@@@  @@\\  .           o     /  ",
      "    \\ . @@     _\\ /    .      .-.  /   ",
      "     `.    .    ()---        `-' .'    ",
      "       `-.    ./ |  .   o     .-'      ",
      "          `--./   .       .--'         ",
      "              `----------'             "};
  static char background21[21][43] = {
      "                .----------.               ",
      "           .---'   O   . .  `---.          ",
      "        .-'@ @@@@@@  .  @@@@@    `-.       ",
      "      .'@@  @@@@@@@@@  @@@@@@@   .  `.     ",
      "     /   o  @@@@@@@@@  @@@@@@@      . \\    ",
      "    /@  o   @@@@@@@@@.  @@@@@@@   O    \\   ",
      "   /@@@  .   @@@@@@o   @@@@@@@@@@     @@\\  ",
      "  /@@@@@            . @@@@@@@@@@@@@ o @@@\\ ",
      " .@@@@@ O  `.-./ .     @@@@@@@@@@@@    @@ .",
      " | @@@@   --`-'      o    @@@@@@@@ @@@@   |",
      " |@ @@@       `   o     .  @@  . @@@@@@@  |",
      " |      @@  @        .-.    @@@  @@@@@@@  |",
      " `  . @       @@@    `-'  . @@@@  @@@@  o '",
      "  \\     @@   @@@@@ .         @@  .       / ",
      "   \\   @@@@  @\\@@    /  . O   .    o  . /  ",
      "    \\o  @@     \\ \\  /       .   .      /   ",
      "     \\    .    .\\.-.___  .     .  .-. /    ",
      "      `.         `-'             `-'.'     ",
      "        `-.  o  / |    o   O  .  .-'       ",
      "           `---.    .    .  .---'          ",
      "                `----------'               "};
  static char background22[22][45] = {
      "                .------------.               ",
      "            .--'   o     . .  `--.           ",
      "         .-'    .    O   .      . `-.        ",
      "       .'@    @@@@@@@   .  @@@@@     `.      ",
      "     .'@@@  @@@@@@@@@@@   @@@@@@@  .   `.    ",
      "    /     o @@@@@@@@@@@   @@@@@@@      . \\   ",
      "   /@@  o   @@@@@@@@@@@.   @@@@@@@   O    \\  ",
      "  /@@@@   .   @@@@@@@o    @@@@@@@@@@    @@@\\ ",
      "  |@@@@@               . @@@@@@@@@@@@  @@@@| ",
      " /@@@@@  O  `.-./  .      @@@@@@@@@@@   @@  \\",
      " | @@@@    --`-'      o    . @@@@@@@ @@@@   |",
      " |@ @@@  @@  @ `   o  .-.     @@  . @@@@@@  |",
      " \\             @@@    `-'  .   @@@  @@@@@@  /",
      "  | . @  @@   @@@@@ .          @@@@  @@@ o | ",
      "  \\     @@@@  @\\@@    /  .  O   @@ .     . / ",
      "   \\  o  @@     \\ \\  /          . . o     /  ",
      "    \\      .    .\\.-.___   .  .  .  .-.  /   ",
      "     `.           `-'              `-' .'    ",
      "       `.    o   / |     o   O   .   .'      ",
      "         `-.    /     .      .    .-'        ",
      "            `--.        .     .--'           ",
      "                `------------'               "};
  static char background23[23][47] = {
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
      "                 `------------'                "};

  static char background24[24][49] = {
      "                  .------------.                 ",
      "             .---' o     .  .   `---.            ",
      "          .-'   .    O    .       .  `-.         ",
      "        .'@   @@@@@@@   .   @@@@@       `.       ",
      "      .'@@  @@@@@@@@@@@    @@@@@@@   .    `.     ",
      "     /    o @@@@@@@@@@@    @@@@@@@       .  \\    ",
      "    /@  o   @@@@@@@@@@@.    @@@@@@@   O      \\   ",
      "   /@@@   .   @@@@@@@o     @@@@@@@@@@     @@@ \\  ",
      "  /@@@@@               .  @@@@@@@@@@@@@ o @@@@ \\ ",
      "  |@@@@  O  `.-./  .       @@@@@@@@@@@@    @@  | ",
      " / @@@@    --`-'       o      @@@@@@@@ @@@@     \\",
      " |@ @@@     @  `           .   @@     @@@@@@@   |",
      " |      @           o          @      @@@@@@@   |",
      " \\       @@            .-.      @@@    @@@@  o  /",
      "  | . @        @@@     `-'    . @@@@           | ",
      "  \\      @@   @@@@@ .            @@   .        / ",
      "   \\    @@@@  @\\@@    /  .   O    .     o   . /  ",
      "    \\ o  @@     \\ \\  /          .    .       /   ",
      "     \\     .    .\\.-.___    .      .   .-.  /    ",
      "      `.          `-'                 `-' .'     ",
      "        `.   o   / |      o    O   .    .'       ",
      "          `-.   /      .       .     .-'         ",
      "             `---.       .      .---'            ",
      "                  `------------'                 "};
  static char background29[29][59] = {
      "                      .--------------.                     ",
      "                 .---'  o        .    `---.                ",
      "              .-'    .    O  .         .   `-.             ",
      "           .-'     @@@@@@       .             `-.          ",
      "         .'@@   @@@@@@@@@@@       @@@@@@@   .    `.        ",
      "       .'@@@  @@@@@@@@@@@@@@     @@@@@@@@@         `.      ",
      "      /@@@  o @@@@@@@@@@@@@@     @@@@@@@@@     O     \\     ",
      "     /        @@@@@@@@@@@@@@  @   @@@@@@@@@ @@     .  \\    ",
      "    /@  o      @@@@@@@@@@@   .  @@  @@@@@@@@@@@     @@ \\   ",
      "   /@@@      .   @@@@@@ o       @  @@@@@@@@@@@@@ o @@@@ \\  ",
      "  /@@@@@                  @ .      @@@@@@@@@@@@@@  @@@@@ \\ ",
      "  |@@@@@    O    `.-./  .        .  @@@@@@@@@@@@@   @@@  | ",
      " / @@@@@        --`-'       o        @@@@@@@@@@@ @@@    . \\",
      " |@ @@@@ .  @  @    `    @            @@      . @@@@@@    |",
      " |   @@                         o    @@   .     @@@@@@    |",
      " |  .     @   @ @       o              @@   o   @@@@@@.   |",
      " \\     @    @       @       .-.       @@@@       @@@      /",
      "  |  @    @  @              `-'     . @@@@     .    .    | ",
      "  \\ .  o       @  @@@@  .              @@  .           . / ",
      "   \\      @@@    @@@@@@       .                   o     /  ",
      "    \\    @@@@@   @@\\@@    /        O          .        /   ",
      "     \\ o  @@@       \\ \\  /  __        .   .     .--.  /    ",
      "      \\      .     . \\.-.---                   `--'  /     ",
      "       `.             `-'      .                   .'      ",
      "         `.    o     / | `           O     .     .'        ",
      "           `-.      /  |        o             .-'          ",
      "              `-.          .         .     .-'             ",
      "                 `---.        .       .---'                ",
      "                      `--------------'                     "};
  static char background32[32][65] = {
      "                         .--------------.                        ",
      "                   .----'  o        .    `----.                  ",
      "                .-'     .    O  .          .   `-.               ",
      "             .-'      @@@@@@       .              `-.            ",
      "           .'@     @@@@@@@@@@@       @@@@@@@@    .   `.          ",
      "         .'@@    @@@@@@@@@@@@@@     @@@@@@@@@@         `.        ",
      "       .'@@@ o   @@@@@@@@@@@@@@     @@@@@@@@@@      o    `.      ",
      "      /@@@       @@@@@@@@@@@@@@  @   @@@@@@@@@@  @@     .  \\     ",
      "     /            @@@@@@@@@@@   .  @@   @@@@@@@@@@@@     @@ \\    ",
      "    /@  o     .     @@@@@@ o       @   @@@@@@@@@@@@@@ o @@@@ \\   ",
      "   /@@@                        .       @@@@@@@@@@@@@@@  @@@@@ \\  ",
      "  /@@@@@                     @      .   @@@@@@@@@@@@@@   @@@   \\ ",
      "  |@@@@@     o      `.-./  .             @@@@@@@@@@@@ @@@    . | ",
      " / @@@@@           __`-'       o          @@       . @@@@@@     \\",
      " |@ @@@@ .        @    `    @            @@    .     @@@@@@     |",
      " |   @@       @                    o       @@@   o   @@@@@@.    |",
      " |          @                             @@@@@       @@@       |",
      " |  . .  @      @  @       o              @@@@@     .    .      |",
      " \\            @                .-.      .  @@@  .           .   /",
      "  |    @   @   @      @        `-'                     .       / ",
      "  \\   .      @   @                   .            o            / ",
      "   \\     o          @@@@   .                .                 /  ",
      "    \\       @@@    @@@@@@        .                    o      /   ",
      "     \\     @@@@@   @@\\@@    /         o           .         /    ",
      "      \\  o  @@@       \\ \\  /  ___         .   .     .--.   /     ",
      "       `.      .       \\.-.---                     `--'  .'      ",
      "         `.             `-'       .                    .'        ",
      "           `.    o     / |              O      .     .'          ",
      "             `-.      /  |         o              .-'            ",
      "                `-.           .         .      .-'               ",
      "                   `----.        .       .----'                  ",
      "                         `--------------'                        "};

  static char *pnames[4] = {
      "New Moon",
      "First Quarter",
      "Full Moon",
      "Last Quarter",
  };

  double jd, angphase, cphase, aom;
  double phases[2], which[2];
  int lin, col, midlin;
  double mcap, yrad, xrad, y, xright, xleft;
  int colright, colleft;
  char c;

  /* Figure out the phase. */
  jd = jdate(gmtime(&t));
  angphase = phase(jd, &cphase, &aom) * 2.0 * PI;
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
    if (angphase >= 0.0 && angphase < PI)
      xleft = mcap * xleft;
    else
      xright = mcap * xright;
    colleft = (int)(xrad + 0.5) + (int)(xleft + 0.5);
    colright = (int)(xrad + 0.5) + (int)(xright + 0.5);

    /* Now output the slice. */
    for (col = 0; col < colleft; ++col)
      putchar(' ');
    for (; col <= colright; ++col) {
      switch (numlines) {
      case 18: c = background18[lin][col]; break;
      case 19: c = background19[lin][col]; break;
      case 21: c = background21[lin][col]; break;
      case 22: c = background22[lin][col]; break;
      case 23: c = background23[lin][col]; break;
      case 24: c = background24[lin][col]; break;
      case 29: c = background29[lin][col]; break;
      case 32: c = background32[lin][col]; break;
      default: c = '@';
      }
      if (c != '@')
        putchar(c);
      else {
        putchar('@');
      }
    }

    if (numlines <= 27) {
      /* Output the end-of-line information, if any. */
      if (lin == midlin - 2) {
        fputs("\t ", stdout);
        printf("%s +", pnames[(int)(which[0] * 4.0 + 0.001)]);
      } else if (lin == midlin - 1) {
        fputs("\t ", stdout);
        putseconds((int)((jd - phases[0]) * SECSPERDAY));
      } else if (lin == midlin) {
        fputs("\t ", stdout);
        printf("%s -", pnames[(int)(which[1] * 4.0 + 0.001)]);
      } else if (lin == midlin + 1) {
        fputs("\t ", stdout);
        putseconds((int)((phases[1] - jd) * SECSPERDAY));
      }
    }

    putchar('\n');
  }
}

/* Main program. */

int main(int argc, char **argv) {
  time_t t;
  char buf[100];
  int numlines, argn;
  char *usage = "usage:  %s  [-l <lines>]  [<date/time>]\n";

  /* Parge args. */
  argn = 1;
  /* Check for -l flag. */
  numlines = DEFAULTNUMLINES;
  if (argc - argn >= 1) {
    if (argv[argn][0] == '-') {
      if (argv[argn][1] != 'l' || argv[argn][2] != '\0') {
        fprintf(stderr, usage, argv[0]);
        exit(1);
      } else {
        if (argc - argn < 2) {
          fprintf(stderr, usage, argv[0]);
          exit(1);
        }
        if (sscanf(argv[argn + 1], "%d", &numlines) != 1) {
          fprintf(stderr, usage, argv[0]);
          exit(1);
        }
        argn += 2;
      }
    }
  }

  /* Figure out what date and time to use. */
  if (argc - argn == 0) {
    /* No arguments present - use the current date and time. */
    t = time(0);
  } else if (argc - argn == 1 || argc - argn == 2 || argc - argn == 3) {
    /* One, two, or three args - use them. */
    strcpy(buf, argv[argn]);
    if (argc - argn > 1) {
      strcat(buf, " ");
      strcat(buf, argv[argn + 1]);
      if (argc - argn > 2) {
        strcat(buf, " ");
        strcat(buf, argv[argn + 2]);
      }
    }
    t = date_parse(buf);
  } else {
    /* Too many args! */
    fprintf(stderr, usage, argv[0]);
    exit(1);
  }

  putmoon(t, numlines);
}

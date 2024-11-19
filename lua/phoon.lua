#!/usr/bin/lua
--[[ phoon - show the phase of the moon
**
** ver  date   who remarks
** --- ------- --- -------------------------------------------------------------
** 04A 02feb24 OW  Revision of the source code to be c99/POSIX2008
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
--]]

require("astro")
require("date_parse")

local function unix_to_julian(t)
  return (t / 86400.0 + 2440587.5)
end

local function printf(fmt, ...) io.write(string.format(fmt, ...)) end

-- If you change the aspect ratio, the canned backgrounds won't work.
local ASPECTRATIO = 0.5

local function putseconds(secs)
  local days, hours, minutes

  secs    = math.floor(secs)
  days    = secs // 86400
  secs    = secs - days * 86400
  hours   = secs // 3600
  secs    = secs - hours * 3600
  minutes = secs // 60
  secs    = secs - minutes * 60

  printf("\t %d %2d:%02d:%02d", days, hours, minutes, secs)
end

local numlines = 23

local function putmoon(t)
  local bg = {
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
  }
  local qlits = {
    "New Moon +",
    "First Quarter +",
    "Full Moon +",
    "Last Quarter +",
  }
  local nqlits = {
    "New Moon -",
    "First Quarter -",
    "Full Moon -",
    "Last Quarter -",
  }

  local jd, angphase, cphase, aom -- double
  local phases = {} -- double[2]
  local lin, col, midlin, which; --int
  local mcap, yrad, xrad, y, xright, xleft; --double
  local colright, colleft -- int

  -- Figure out the phase
  jd = unix_to_julian(t);
  angphase, cphase, aom = phase(jd);
  angphase = angphase * math.pi * 2
  mcap = -math.cos(angphase);

  -- Figure out how big the moon is.
  yrad = numlines / 2;
  xrad = yrad / ASPECTRATIO;

  -- Figure out some other random stuff.
  midlin = numlines // 2;
  phases, which = phasehunt2(jd);

  -- Now output the moon, a slice at a time. */
  for lin = 1, numlines-1 do
    -- Compute the edges of this slice.
    y = lin - 0.5 - yrad;
    xright = xrad * math.sqrt(1.0 - (y * y) / (yrad * yrad));
    xleft = -xright;
    if (angphase >= 0 and angphase < math.pi) then xleft = mcap * xleft
    else xright = mcap * xright end
    colleft  = math.floor(xrad + 0.5) + math.floor(xleft + 0.5)
    colright = math.floor(xrad + 0.5) + math.floor(xright + 0.5)

    -- Now output the slice
    for col = 1, colleft do io.write(' ') end
    for col = colleft, colright+1 do
      io.write(string.sub(bg[lin], col, col))
    end
    -- Output the end-of-line information, if any.
    if (lin == midlin - 2) then printf("\t %-16s", qlits[math.floor(which + 0.001)+1])
    elseif lin == midlin - 1 then putseconds((jd - phases[1]) * 86400)
    elseif lin == midlin then printf("\t %-16s", nqlits[math.floor(which + 0.001)+1])
    elseif lin == midlin + 1 then putseconds((phases[2] - jd) * 86400) end

    print('')
  end
end

if #arg > 1 then io.write(io.stderr, "usage: [<date/time>]\n"); os.exit(1); end
putmoon(arg[1] and date_parse(arg[1]) or os.time())

--[[ Adapted from "moontool.c" by John Walker
Quoting from the original:

  The algorithms used in this program to calculate the positions Sun and
  Moon as seen from the Earth are given in the book "Practical Astronomy
  With  Your  Calculator"  by  Peter  Duffett-Smith,   Second   Edition,
  Cambridge University Press, 1981.  Ignore the word "Calculator" in the
  title;  this  is  an  essential  reference  if  you're  interested  in
  developing  software  which  calculates  planetary  positions, orbits,
  eclipses, and  the  like.   If  you're  interested  in  pursuing  such
  programming, you should also obtain:

    "Astronomical  Formulae for Calculators" by Jean Meeus, Third Edition,
    Willmann-Bell, 1985.  A must-have.

    "Planetary  Programs  and  Tables  from  -4000  to  +2800"  by  Pierre
    Bretagnon  and Jean-Louis Simon, Willmann-Bell, 1986.  If you want the
    utmost  (outside  of  JPL)  accuracy  for  the  planets,  it's   here.

    "Celestial BASIC" by Eric Burgess, Revised Edition, Sybex, 1985.  Very
    cookbook oriented, and many of the algorithms are hard to dig  out  of
    the turgid BASIC code, but you'll probably want it anyway.

See http://www.fourmilab.ch/moontool/
*/]]

local epoch = 2444238.5      -- 1980 January 0.0

--  Constants defining the Sun's apparent orbit
local elonge = 278.833540    -- Ecliptic longitude of the Sun at epoch 1980.0
local elongp = 282.596403    -- Ecliptic longitude of the Sun at perigee
local eccent = 0.016718      -- Eccentricity of Earth's orbit

-- Elements of the Moon's orbit, epoch 1980.0
local synmonth = 29.53058868 -- Synodic month (new Moon to new Moon)

-- Handy mathematical functions
local abs = math.abs
local floor = math.floor
local sin = math.sin
local cos = math.cos
local torad = math.rad
local todeg = math.deg
local function fixangle(a) return (a-360.0 * (floor(a / 360.0))) end
local function dsin(x) return (sin(torad(x))) end
local function dcos(x) return (sin(torad(x))) end

--[[ JYEAR:  Convert Julian date to year, month, day, which are
             returned in a table
--]]

local function jyear(td)
  local j,d,y,m = 0,0,0,0

  td = td + 0.5 -- Astronomical to civil
  j = floor(td)
  j = j - 1721119.0
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
  if (m < 10.0) then
    m = m + 3;
  else
    m = m - 9;
    y = y + 1;
  end
  return y, m, d
end

--[[
 MEANPHASE   Calculates time of the mean new Moon for a given
             base date.  This argument K to this function is
	           the precomputed synodic month index, given by:

		 K = (year - 1900) * 12.3685

	           where year is expressed as a year and fractional year.
--]]
local function meanphase(sdate, k)
  -- Time in Julian centuries from 1900 January 0.5
  local t = (sdate - 2415020.0) / 36525;

  return 2415020.75933 + synmonth * k
      + 0.0001178 * (t^2)
      - 0.000000155 * (t^3)
      + 0.00033 * dsin(166.56 + 132.87 * t - 0.009173 * (t^2));
end

--[[
 TRUEPHASE  Given a K value used to determine the mean phase of the new moon,
            and a phase selector (0.0, 0.25, 0.5, 0.75), obtain the true,
            corrected phase time.
--]]
local function truephase(k, pha)
  local t, t2, t3, pt, m, mprime, f;
  local apcor = false

  k = k + pha        -- Add phase to new moon time
  t = k / 1236.85    -- Time in Julian centuries from 1900 January 0.5
  t2 = t * t         -- Square for frequent use
  t3 = t2 * t;       -- Cube for frequent use
  pt = 2415020.75933 -- Mean time of phase
      + synmonth * k + 0.0001178 * t2 - 0.000000155 * t3
      + 0.00033 * dsin(166.56 + 132.87 * t - 0.009173 * t2);

  m = 359.2242       -- Sun's mean anomaly
      + 29.10535608 * k - 0.0000333 * t2 - 0.00000347 * t3;
  mprime = 306.0253  -- Moon's mean anomaly
      + 385.81691806 * k + 0.0107306 * t2 + 0.00001236 * t3;
  f = 21.2964        -- Moon's argument of latitude
      + 390.67050646 * k - 0.0016528 * t2 - 0.00000239 * t3;
  if ((pha < 0.01) or (abs(pha - 0.5) < 0.01)) then
    -- Corrections for New and Full Moon
    pt = pt + (0.1734 - 0.000393 * t) * dsin(m)
        + 0.0021 * dsin(2 * m) - 0.4068 * dsin(mprime)
        + 0.0161 * dsin(2 * mprime) - 0.0004 * dsin(3 * mprime)
        + 0.0104 * dsin(2 * f) - 0.0051 * dsin(m + mprime)
        - 0.0074 * dsin(m - mprime) + 0.0004 * dsin(2 * f + m)
        - 0.0004 * dsin(2 * f - m) - 0.0006 * dsin(2 * f + mprime)
        + 0.0010 * dsin(2 * f - mprime) + 0.0005 * dsin(m + 2 * mprime);
    apcor = true;
  elseif ((abs(pha - 0.25) < 0.01 or (abs(pha - 0.75) < 0.01))) then
    pt = pt + (0.1721 - 0.0004 * t) * dsin(m)
        + 0.0021 * dsin(2 * m) - 0.6280 * dsin(mprime)
        + 0.0089 * dsin(2 * mprime) - 0.0004 * dsin(3 * mprime)
        + 0.0079 * dsin(2 * f) - 0.0119 * dsin(m + mprime)
        - 0.0047 * dsin(m - mprime) + 0.0003 * dsin(2 * f + m)
        - 0.0004 * dsin(2 * f - m) - 0.0006 * dsin(2 * f + mprime)
        + 0.0021 * dsin(2 * f - mprime) + 0.0003 * dsin(m + 2 * mprime)
        + 0.0004 * dsin(m - 2 * mprime) - 0.0003 * dsin(2 * m + mprime);
    if (pha < 0.5) then
      -- First quarter correction
      pt = pt + 0.0028 - 0.0004 * dcos(m) + 0.0003 * dcos(mprime);
    else
      -- Last quarter correction
      pt = pt + -0.0028 + 0.0004 * dcos(m) - 0.0003 * dcos(mprime);
    end
    apcor = true;
  end
  if (not apcor) then
    io.write(io.stderr, "TRUEPHASE called with invalid phase selector.\n");
    os.exit(1);
  end
  return pt
end

--[[
  PHASEHUNT2  --  Find time of phases of the moon which surround
	               	the current date.  Two phases are found.
--]]
function phasehunt2(sdate)
  local adate = sdate - 45
  local k1, k2, nt1, nt2
  local yy, mm, dd = jyear(adate)
  local phases = {}
  local which = 0

  k1 = floor((yy + ((mm - 1) * (1.0 / 12.0)) - 1900) * 12.3685);

  nt1 = meanphase(adate, k1)
  adate = nt1
  while true do
    adate = adate + synmonth;
    k2 = k1 + 1
    nt2 = meanphase(adate, k2);
    if (nt1 <= sdate and nt2 > sdate) then break end
    nt1 = nt2
    k1 = k1 + 1
  end

  phases[1] = truephase(k1, 0.0);
  phases[2] = truephase(k1, 0.25);
  if (phases[2] <= sdate) then
    which = which + 1;
    phases[1] = phases[2];
    phases[2] = truephase(k1, 0.5)
    if (phases[2] <= sdate) then
      phases[1] = phases[2];
      which = which + 1;
      phases[2] = truephase(k1, 0.75)
      if (phases[2] <= sdate) then
        phases[1] = phases[2];
        phases[2] = truephase(k2, 0.0);
        which = which + 1;
      end
    end
  end
  return phases, which
end

-- KEPLER  --	Solve the equation of Kepler.
local function kepler(m, ecc)
  m = torad(m)
  local e = m
  local delta

  delta = e - ecc * sin(e) - m;
  e = e - delta / (1 - ecc * cos(e));
  while abs(delta) > 1E-6 do
    delta = e - ecc * sin(e) - m;
    e = e - delta / (1 - ecc * cos(e));
  end
  return e;
end

--[[
  PHASE  --  Calculate phase of moon as a fraction:

  	The argument is the time for which the phase is requested,
  	expressed as a Julian date and fraction.  Returns the terminator
  	phase angle as a percentage of a full circle (i.e., 0 to 1),
  	and stores into pointer arguments the illuminated fraction of
  	the Moon's disc, the Moon's age in days and fraction, the
 	  distance of the Moon from the centre of the Earth, and the
 	  angular diameter subtended by the Moon as seen by an observer
 	  at the centre of the Earth.

  pphase:		Illuminated fraction
  mage:		Age of moon in days
--]]
function phase(pdate)
  local Day, M, Ec, Lambdasun, ml, MM, Ev, Ae, MmP, lP, lPP, MoonAge

  -- Calculation of the Sun's position

  Day = pdate - epoch; -- Date within epoch */
  M = fixangle(fixangle((360 / 365.2422) * Day) + elonge - elongp); -- Convert from perigee co-ordinates to epoch 1980.0
  Ec = kepler(M, eccent); -- Solve equation of Kepler */
  Ec = math.sqrt((1 + eccent) / (1 - eccent)) * math.tan(Ec / 2);
  Ec = 2 * todeg(math.atan(Ec)); -- True anomaly
  Lambdasun = fixangle(Ec + elongp); -- Sun's geocentric ecliptic longitude
  ml = fixangle(13.1763966 * Day + 64.975464); -- Moon's mean lonigitude at the epoch
  -- Moon's mean anomaly
  MM = fixangle(ml - 0.1114041 * Day - 349.383063); -- 349: Mean longitude of the perigee at the epoch
  Ev = 1.2739 * sin(torad(2 * (ml - Lambdasun) - MM)); -- Evection */
  Ae = 0.1858 * sin(torad(M)); -- Annual equation
  MmP = MM + Ev - Ae - (0.37 * sin(torad(M))); -- Corrected anomaly
  -- Corrected longitude
  lP = ml + Ev + (6.2886 * sin(torad(MmP))) - Ae + (0.214 * sin(torad(2 * MmP)));
  lPP = lP + (0.6583 * sin(torad(2 * (lP - Lambdasun)))); -- True longitude

  MoonAge = lPP - Lambdasun; -- Age of the Moon in degrees

  -- print(pdate, fixangle(MoonAge) / 360.0)
  return fixangle(MoonAge) / 360.0, (1 - cos(torad(MoonAge))) / 2,(synmonth * (fixangle(MoonAge) / 360.0))
end


#!/usr/bin/lua
require("astro")
require("date_parse")
local halfmonth = 14.76529434 -- Half Synodic month (new Moon to full Moon)

local phasenames= { "New", "Waxing Crescent", "First Quarter", "Waxing Gibbous", "Full", "Waning Gibbous", "Last Quarter", "Waning Crescent" };
local emojis       = {"🌑", "🌒", "🌓", "🌔", "🌕",  "🌖", "🌗", "🌘"};
local emojis_south = {"🌑", "🌘", "🌗", "🌖", "🌕",  "🌔", "🌓", "🌒"};

local HELPTXT = "mprintf [-h] [-t TIME] [FORMAT]"
local help = HELPTXT .. [[
-f formats:
%a Moon Age\t %J Julian Day
%e Emoji\t %s Emoji of phase (Southern Hemisphere)
%p Phase Name\t %P Illuminated Percent
%% Percent Sign\t %n Newline]]

-- JDATE  --  Convert internal GMT date and time to Julian day and fraction.
local function jdate(t)
	local c, m
	local y = t.year

	m = t.month;
	if (m > 2) then m = m - 3 else m = m + 9; y = y - 1 end
	c = y // 100 -- Compute century
	y = y - 100 * c;
	return (t.day + (c * 146097) // 4 + (y * 1461) // 4 + (m * 153 + 2) // 5 + 1721119);
end

-- JTIME -- Convert internal GMT date and time to astronomical Julian time (i.e. Julian date plus day fraction). double jtime (struct tm *t)
local function jtime(t) return ((jdate(t) - 0.5) + (t.sec + 60 * (t.min + 60 * t.hour)) / 86400.0) end

-- PHASEINDEX --  Return phase based on Illuminated Fraction of the moon and age of the moon
local function phaseindex(ilumfrac, mage)
  if     ilumfrac < 0.04 then return 0
  elseif ilumfrac > 0.96 then return 4
  elseif ilumfrac > 0.46 and ilumfrac < 0.54 then return (mage > halfmonth) and 6 or 2
  elseif ilumfrac > 0.54 and ilumfrac < 0.96 then return (mage > halfmonth) and 5 or 3
  else   return (mage > halfmonth) and 7 or 1;
  end
end

--void mprintf(char *fmt, double ilumfrac, double mage, struct tm *time)
local function mprintf(fmt, ilumfrac, mage, time)
  local putc = io.write
  local function printf(fmt, ...) io.write(string.format(fmt, ...)) end
  local indx = phaseindex(ilumfrac, mage) + 1
  for ps in string.gmatch(fmt, "[%%.]?.") do
    if not string.find(ps, "%%") then putc(ps)
    elseif ps == '%%' then putc('%')
    elseif ps == '%n' then putc('\n')
    elseif ps == '%t' then putc('\t')
    elseif ps == '%a' then printf("%2.1f", mage)
    elseif ps == '%J' then printf("%.6f", jtime(time))
    elseif ps == '%e' then putc(emojis[indx])
    elseif ps == '%s' then putc(emojis_south[indx])
    elseif ps == '%p' then putc(phasenames[indx])
    elseif ps == '%P' then printf("%2.1f", ilumfrac*100)
    elseif ps == '%N' then printf("%d", indx)
    else io.write(io.stderr, "Unknown flag\n"); os.exit(1) end
  end
  print('');
end

if arg[1] == "-t" then now = date_parse(arg[2]); fmtind = 3
elseif arg[2] == "-t" then now = date_parse(arg[3]); fmtind = 1
else now = os.time(); fmtind = 1 end
fmtstr = arg[fmtind] and arg[fmtind] or "%p %e (%P%%)";
gm = os.date("!*t", now)
cdtd = jtime(gm)
un, ilfrac, moonage = phase(cdtd)
mprintf(fmtstr, ilfrac, moonage, gm);

-- date_parse - parse string dates into internal form
-- See LICENSE

function date_parse(str)
  local h = io.popen("date -ud \"" .. str .. "\" +%s", 'r')
  local k = h:read("*a")
  h:close()
  return tonumber(k)
end

--[[ TODO:

local formats = {
  -- d/m/y format
  "%d/%m/%Y %T", "%d/%m/%Y",
  -- d-m-y format
  "%d-%b-%Y %T", "%d-%b-%Y",
  -- DD MM YY HH:MM:SS
  "%d %b %Y %r", "%d %b %Y %T",
  "%d %b %Y %H:%M", "%d %b %Y",
  -- ctime(3) format - zonename
  "%a %b %d %T %Y", "%a %b %d",
}

local ifmts = {
  "%r",
  "%T",
  "%H:%M",
  "%b %d %T",
  "%b %d",
}

time_t date_parse(char *str)
{
  if string.sub(str, 1, 1) == '@' then return tonumber(string.sub(str, 2)) end

  extern long timezone;
  tzset();

  int indx = 0;
  static struct tm tm; // static = bss = zeroed
  if ((*str == '+' || *str == '-') && (str[1] != '+' && str[1] != '-')) {
    // Default initilization for gmtime at unix epoch
    tm.tm_year = 70;
    tm.tm_mday = 1;
    tm.tm_wday = 4;
    tm.tm_zone = "UTC"; // GMT on glibc, UTC on musl

    if (!strptime(str + 1, "%T", &tm) && 
        !strptime(str + 1, "%H:%M", &tm) && 
        (!strptime(str + 1, "%dd %H:%M", &tm) || !(++tm.tm_mday)))
      dprintf(2, "Unknown date format: `%s`\n", str), exit(2);

    time_t now = time(0);
    now += (*str == '+') ? mktime(&tm) : -mktime(&tm);

    return now - timezone;
  }

  while (!strptime(str, formats[indx], &tm) && formats[++indx]) ;

  -- ABANDON ALL HOPE; YE WHO ENTER HERE
  -- Initiliaze tm after this so we don't show stuff for the year 1900.

  if (!formats[indx]) {
    time_t now = time(0);
    tm = *localtime(&now);
    indx = tm.tm_hour = tm.tm_min = tm.tm_sec = 0;

    while (!strptime(str, ifmts[indx], &tm))
      if (!ifmts[++indx])
        dprintf(2, "Unknown date format: `%s`\n", str), exit(2);

    return os.time(tm);
  }

  -- printf("%s", asctime(&tm));
  return mktime(&tm) - timezone;
}
--]]
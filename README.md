# Moontool - A modernization and adaptation of '80s astronomical trinkets

The idea came from [this](https://www.fourmilab.ch/moontool/), and most
of the astronomical code is from the original moontool

## mprintf

This one I actually wrote (although, heavily adapted from the original moontool
code)

-f formatting specifiers:
```
| character | definition                  | example         |
|-----------+-----------------------------+-----------------|
| %a        | Age (Days since new moon)   |           15.20 |
| %e        | Emoji (Northern Hemisphere) | 🌘               |
| %s        | Emoji (Southern Hemisphere) | 🌒               |
| %J        | Julian Day                  | 2460494.401019  |
| %N        | Phase Number                |               5 |
| %P        | Illuminated Percent         | 10%             |
| %p        | Phase name                  | New             |
| %%        | percent sign                | %               |
| %n        | Newline                     | \n              |
| %t        | Tab                         | \t              |
```

## phoon

[From Here](https://www.acme.com/software/phoon/) Originally Written by
John Walker

>>> phoon is a program to display the PHase of the mOON. Unlike other
such programs, phoon *shows* you the phase First written in Pascal
/TOPS-20 at CMU in 1979; Then translated it to Ratfor/Software Tools
in 1981; and now it's in C/Unix.

## globe

[From Here](https://www.acme.com/software/Globe/) Originally Written by
John Walker

## timecalc

A simple test of the date parsing stuff, practically a debug tool


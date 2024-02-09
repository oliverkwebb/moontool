#include <time.h>
// From Toybox: Orig Author Rob landley, lib/lib.c (millitime)

int mpp = 1000;

long microtime(void)
{
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec*(1000*mpp)+ts.tv_nsec/mpp;
}


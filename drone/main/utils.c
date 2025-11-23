#include "utils.h"

uint64_t getHRTime() {
  static struct timeval tv_now;
  gettimeofday(&tv_now, NULL);

  return (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec;
}

/**
 * Time in seconds (High Resolution Time)
 */
float getHRTimeF() { return (float)(getHRTime() / 1000000.0); }

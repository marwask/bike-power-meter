#include <bluefruit.h>
#include "bluetooth.h"

#ifndef htons
  #define htons(x) (((x)<< 8 & 0xFF00) | \
                  ((x)>> 8 & 0x00FF))
#endif

#ifndef ntohs
  #define ntohs(x) htons(x)
#endif

float average (int *data, int len) {
  long sum = 0L;
  for (int i = 0; i < len; i++) {
    sum += data[i];
  }
  return ((float) sum) / len;
}
#ifndef STRUCTS_H
#define STRUCTS_H

#include <bluefruit.h>

typedef struct {
  word flags;
  uint16_t instantaneousPower;
  uint16_t  cumulativeCrankRevs;
  uint16_t  lastCrankEventTime;
} powerDataEntry;

struct __attribute((__packed__)) bluetoothDebugDataEntry {
  short angle;
  short cadence;
  long normalizedScaleValue;
  short power;
  bool isRotated;
};

#endif //STRUCTS_H
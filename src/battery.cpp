#include <Arduino.h>
#include "battery.h"
#include <bluefruit.h>


float Battery::readVBAT(void) {
  analogReference(AR_INTERNAL_3_0);
  analogReadResolution(12); // Can be 8, 10, 12 or 14
  delay(1);
  float raw = analogRead(vbat_pin);
  analogReference(AR_DEFAULT);
  analogReadResolution(10);
  return raw * REAL_VBAT_MV_PER_LSB;
}

uint8_t Battery::milliVoltsToPercent(float mvolts) {
  if (mvolts < 3300)
    return 0;
  if (mvolts < 3600) {
    mvolts -= 3300;
    return mvolts / 30;
  }
  mvolts -= 3600;
  return 10 + (mvolts * 0.15F );
}

float Battery::getBatteryPercentage() {
  float vbat_mv = readVBAT();
  uint8_t vbat_per = milliVoltsToPercent(vbat_mv);
  return vbat_per;
}

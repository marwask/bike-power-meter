#include <Arduino.h>
#include "scale.h"
#include "HX711.h"
#include "config.h"


void Scale::init() {
    hx711.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    // hx711.set_gain(64);
    calibrateBaseOffset();
}

boolean Scale::isReady(void) {
    return hx711.is_ready();
}

int Scale::read(void) {
    if(initialConfiguration) {
        return hx711.read_average(20);
    } else {
        return hx711.read();
    }
}

boolean Scale::isCalibrating(void) {
    return calibrating;
}

int Scale::getBaseOffset(void) {
    return baseOffset;
}

void Scale::setBaseOffset(long value) {
    baseOffset = value;
}

void Scale::calibrateBaseOffset(void) {
    Serial.println("Calibrating base offset");
    if (calibrating == true) {
        return;
    }
    calibrating = true;
    for (int i = 1; i <= CALIBRATION_RETRIES; i++) {
        // averageTemperature += mpu.getTemp();
        digitalToggle(LED_RED);
        long partialOffset = hx711.read_average(200);
        offsets[i] = partialOffset;

        Serial.print("calibrating, please do not touch the crank - ");
        Serial.print(i);
        Serial.print("/");
        Serial.print(CALIBRATION_RETRIES);
        Serial.print(", partialOffset: ");
        Serial.println(partialOffset);
    }
    baseOffset = (offsets[CALIBRATION_RETRIES] + offsets[CALIBRATION_RETRIES-1] + offsets[CALIBRATION_RETRIES-2]) / 3;
    Serial.print("base offset: ");
    Serial.println(baseOffset);
    calibrating = false;
}

void Scale::restartScale(void) {
   hx711.power_down();
   delay(100);
   hx711.power_up();
}
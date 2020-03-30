#include "bluefruit.h"
#include "MPU9250.h"
#include "mpu.h"

void Mpu::init(void) {
  mpu.setup();
  // mpu.calibrateAccelGyro();
  // mpu.printRawData();
  // mpu.printRollPitchYaw();
  // mpu.printCalibration();

  mpu.setAccBias(0, 0.06262);
  mpu.setAccBias(1, 0.0101471);
  mpu.setAccBias(2, 0.083459);
  mpu.setGyroBias(0, -0.28);
  mpu.setGyroBias(1, 1.39);
  mpu.setGyroBias(2, -1.02);
  mpu.setMagBias(0, 0);
  mpu.setMagBias(1, 0);
  mpu.setMagBias(2, 0);

  // mpu.printRawData();
  // mpu.printRollPitchYaw();
  // mpu.printCalibration();
  mpu.update();
}

void Mpu::update(void) {
    if(mpu.available()) {
        mpu.update();
    }
}

float Mpu::getPitch(void) {
    return mpu.getPitch();
}

boolean Mpu::detectRotation(int value) {
    if (value > 30) {
        aboveZero = true;
    } else if (value < -30) {
        belowZero = true;
    }
    if (aboveZero && belowZero) {
        belowZero = false;
        aboveZero = false;
        halfRotation++;
    }
    if (halfRotation == 4) { // 2
        halfRotation = 0;
        return true;
    } else {
        return false;
    }
}
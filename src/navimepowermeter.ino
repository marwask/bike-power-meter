#include <bluefruit.h>
#include "HX711.h"
#include "bluetooth.h"
#include "battery.h"
#include "scale.h"
#include "mpu.h"
#include "utils.h"
#include "structs.h"
#include "config.h"

//////////////////////////

Mpu mpu;
Scale scale;
Bluetooth bluetooth;
Battery battery;

//////////////////////////

static uint16_t cumulativeCrankRevs = 0;
long recentRotationEventTime = millis();
uint32_t lastBluetoothDebugUpdateTime = 0;
float recentAngle = 0;
long reading = 0;
long recentReading = 0;
int scaleReadings[500];
short recentCadence = 0;
short recentPower = 0;
uint8_t scaleValuesCounter = 0;

int calibrationReadings[200];
int scaleCalibrationValuesCounter = 0;
int calibratedScaleValue = 0;
bool selfCalibrated = false;

powerDataEntry powerStructData;

float angle = 0;
long scaleValue = 0;
boolean isRotated = false; 
long loopCounter = 0;

//////////////////////////////////////////////////////////////////

void connectCallback(uint16_t connectionHandle) {
  BLEConnection* connection = Bluefruit.Connection(connectionHandle);
  // connection->requestPHY();
  // connection->requestDataLengthUpdate();
  // connection->requestMtuExchange(32);
  char name[32] = { 0 };
  connection->getPeerName(name, sizeof(name));
  Serial.print("Connected to ");
  Serial.println(name);
  cumulativeCrankRevs = 0;
}

void disconnectCallback(uint16_t connectionHandle, uint8_t reason) {
  (void) connectionHandle;
  (void) reason;
  Serial.print("Disconnected, reason = 0x"); 
  Serial.println(reason, HEX);
  Serial.println("Advertising!");
}

void cccdCallback(uint16_t connectionHandle, BLECharacteristic* chr, uint16_t cccdValue) {    
    Serial.print("cccd callback: ");
    Serial.println(cccdValue);
}

void writeCallback(uint16_t connectionHandle, BLECharacteristic *chr, uint8_t *data, uint16_t len) {
  Serial.print("write callback len: ");
  Serial.println(len);
  Serial.print("write callback data: ");
  Serial.println(*data);

  uint8_t value = 0;
  for (int i = 0; i < len; i++) {
    // Serial.println(data[i], HEX);
    Serial.println(data[i]);
    value = data[i];
  }

  if (value == 12) {
    scale.calibrateBaseOffset();
    bluetooth.cyclingPowerMeasurementCharacteristic.write8(6);
  }
}

//////////////////////////////////////////////////////////////////

uint32_t lastBatteryUpdateTime = 0;

void notifyBatteryInfo() {
  if (millis() - lastBatteryUpdateTime > bluetoothBatteryUpdateInterval) {
    if (Bluefruit.connected()) {
      bluetooth.blebas.write((int)battery.getBatteryPercentage());
      bluetooth.blebas.notify((int)battery.getBatteryPercentage());
      lastBatteryUpdateTime = millis();
    }
  }
}

uint32_t lastDebugUpdateTime = 0;

void showDebug() {
  if(debug) {
    if (millis() - lastDebugUpdateTime > debugUpdateInterval) {
      Serial.print(scaleValue);
      Serial.print(", angle: ");
      Serial.print(angle);
      Serial.print(", buffer: ");
      Serial.println(scaleValuesCounter);
      lastDebugUpdateTime = millis();
    }
  }
}

void sendBluetoothDebug() {
  if(debug) {
    if (millis() - lastBluetoothDebugUpdateTime > bluetoothDebugUpdateInterval) {
      if (Bluefruit.connected()) {
        bluetoothDebugDataEntry bluetoothDebugData;
        bluetoothDebugData.angle = angle;
        bluetoothDebugData.normalizedScaleValue = scaleValue;
        bluetoothDebugData.cadence = recentCadence;
        bluetoothDebugData.isRotated = isRotated;
        bluetoothDebugData.power = recentPower;
        bluetooth.bleuart.write((byte*)&bluetoothDebugData, sizeof(bluetoothDebugData));
        bluetooth.bleuart.flushTXD();
        lastBluetoothDebugUpdateTime = millis();
      }
    }
  }
}

//////////////////////////////////////////////////////////////////

void loop() {
  if (scale.isCalibrating() == true) {
    delay(1000);
    Serial.println("Waiting for the end of calibration process");
    return;
  }
  loopCounter++;


  if (reading == recentReading) {
    return;
  }
  recentReading = reading;

  if (reading > 0) {
    scaleValue = reading - scale.getBaseOffset();
  } else {
    scaleValue = reading + scale.getBaseOffset();
  }
  
  if(initialConfiguration) {
    int scaleValueToKgEquivalent = int(scaleValue / knownWeight);
    Serial.print("place the known weight (" + String(knownWeight) + "kg) on the crank in a horizontal position and set scaleValueToKgEquivalent to: ");
    Serial.println(scaleValueToKgEquivalent);
    delay(100);
    return;
  }
  
  mpu.update();
  angle = mpu.getPitch();

  if (loopCounter % 200 == 0) {
    if (recentAngle == angle) {
      Serial.println("restarting mpu");
      mpu.init();
      Serial.println("mpu has been restarted");
    }
    recentAngle = angle;
  }

  if (loopCounter > 1000) {
    loopCounter = 0;
  }


  // // self calibration
  // if (yAngle > -6 && yAngle < 6) {
  //   // Serial.println(scaleCalibrationValuesCounter);
  //   m_calibration_readings[scaleCalibrationValuesCounter] = reading;
  //   calibratedScaleValue = -average(m_calibration_readings, scaleCalibrationValuesCounter);
  //   scaleCalibrationValuesCounter++;
  //   // if (scaleCalibrationValuesCounter == sizeof(m_calibration_readings) - 10) {
  //   if (scaleCalibrationValuesCounter == 200 - 1) {
  //     scaleCalibrationValuesCounter = 0;
  //     selfCalibrated = true;
  //     power = 0;
  //     recentPower = 0;
  //   }
  // } else {
  //   if(selfCalibrated == true && calibratedScaleValue != 0) {
  //     Serial.print("setting new base scale value: ");
  //     Serial.println(calibratedScaleValue);
  //     scale.set_base_value(calibratedScaleValue);
  //   }
  //   scaleCalibrationValuesCounter = 0;
  //   memset(m_calibration_readings, 0, sizeof(m_calibration_readings));
  //   selfCalibrated = false;
  // }

  if (scaleValuesCounter < sizeof(scaleReadings)) {
    scaleReadings[scaleValuesCounter] = scaleValue;
    scaleValuesCounter++;
  } else {
    scaleValuesCounter = 0;
  }

  showDebug();

  isRotated = mpu.detectRotation(angle);

  if (isRotated == true) {
    unsigned long rotationEventTime = millis();
    long crankRotationTime = (rotationEventTime - recentRotationEventTime) / 2;
    float cadence = 60000 / crankRotationTime;
    int averageCrankRotationScaleValue = average(scaleReadings, scaleValuesCounter);
    float weight = abs(float(averageCrankRotationScaleValue)) / float(scaleValueToKgEquivalent);
    float newtons = weight * 9.81;
    float newtonMetre = newtons * crankLength;
    float power = (newtonMetre * cadence) / 9.5493;
    power = power * 2; // single / dual
    cumulativeCrankRevs += 2;

    recentCadence = cadence;
    recentRotationEventTime = rotationEventTime;
    recentPower = power;

    Serial.println("=====================");
    Serial.print("crankRotationTime: ");
    Serial.println(crankRotationTime);
    Serial.print("cadence: ");
    Serial.println(cadence);
    Serial.print("weight: ");
    Serial.println(weight);
    Serial.print("newtons: ");
    Serial.println(newtons);
    Serial.print("newtonMetre: ");
    Serial.println(newtonMetre);
    Serial.print("power: ");
    Serial.println(power);
    Serial.println("=====================");

    if (Bluefruit.connected())   {
      powerStructData.flags = 0b0000000000100000;
      powerStructData.instantaneousPower = int(power);
      powerStructData.cumulativeCrankRevs = cumulativeCrankRevs;
      powerStructData.lastCrankEventTime = millis();
      if (bluetooth.cyclingPowerMeasurementCharacteristic.notify((const void *)&powerStructData, sizeof(powerStructData))) {
        Serial.print("Cycling Power Measurement updated to: ");
        Serial.println(power);
      } else {
        Serial.println("ERROR: Notify cycling power not set in the CCCD or not connected!");
      }
    }
    scaleValuesCounter = 0;
    memset(scaleReadings, 0, sizeof(scaleReadings));
  }
  
  notifyBatteryInfo();
  delay(6); // 6ms ~167Hz
}

uint32_t lastScaleUpdateTime = 0;

void loopScaleReading() {
    if (scale.isReady()) {
        reading = scale.read();
    } else {
      Serial.print(".");
    }
    delay(13); // 13ms ~80Hz
}

void loopDebug() {
  sendBluetoothDebug();
}

void setup() {
  delay(100);
  Serial.begin(115200);

  Serial.println("\n--------------------------------");
  Serial.println("Navime Bluetooth Bike Power Meter");
  Serial.println("---------------------------------\n");

  Serial.println("Starting Scale");
  scale.init();

  digitalToggle(LED_RED);
  
  if(initialConfiguration) {
    Serial.println("Starting initial configuration");
    Scheduler.startLoop(loopScaleReading);
  } else {
    Serial.println("Starting MPU");
    Wire.begin();
    mpu.init();

    Serial.println("Starting Bluetooth");
    Bluefruit.begin();
    if(debug) {
      Bluefruit.configPrphBandwidth(BANDWIDTH_MAX);
      Scheduler.startLoop(loopDebug);
    } else {
      Bluefruit.configPrphBandwidth(BANDWIDTH_NORMAL);
    }
    Bluefruit.autoConnLed(false);
    Bluefruit.setTxPower(0);

    Bluefruit.Periph.setConnectCallback(connectCallback);
    Bluefruit.Periph.setDisconnectCallback(disconnectCallback);

    bluetooth.init();

    Serial.println("Configuring Bluetooth Cycling Power Service");
    bluetooth.setupCyclingPowerCharacteristic();
    // bluetooth.cpControlPointChar.setCccdWriteCallback(cccd_callback);
    // bluetooth.cpControlPointChar.setWriteCallback(write_callback);

    Serial.println("Starting Bluetooth Advertisement");
    bluetooth.startAdvertising();

    Scheduler.startLoop(loopScaleReading);

    Serial.println("Inited");
    digitalToggle(LED_RED);
  }
}

#include <Arduino.h>
#include <bluefruit.h>
#include "bluetooth.h"
#include "structs.h"
#include "config.h"

// #define BLUETOOTH_MANUFACTURER "Assioma"
// #define BLUETOOTH_MODEL "26615L"
// #define BLUETOOTH_NAME "ASSIOMA26615L"

#define BLUETOOTH_MANUFACTURER "Wahoo"
#define BLUETOOTH_MODEL "KICKR 7DD3"
#define BLUETOOTH_NAME "Wahoo KICKR 7DD3"

// #define BLUETOOTH_MANUFACTURER "Tacx"
// #define BLUETOOTH_MODEL "Flux 03012"
// #define BLUETOOTH_NAME "Tacx Flux 03012"

Bluetooth::Bluetooth() {
  cyclingPowerService = BLEService(UUID16_SVC_CYCLING_POWER);
  cyclingPowerMeasurementCharacteristic = BLECharacteristic(UUID16_CHR_CYCLING_POWER_MEASUREMENT);
  cyclingPowerLocationChararacteristic = BLECharacteristic(UUID16_CHR_SENSOR_LOCATION);
  cyclingPowerFeatureChararacteristic = BLECharacteristic(UUID16_CHR_CYCLING_POWER_FEATURE);
  cyclingPowerControlPointChararacteristic = BLECharacteristic(0x2A66);
}

void Bluetooth::init(void) {
  Bluefruit.setName(BLUETOOTH_NAME);
  bledis.setManufacturer(BLUETOOTH_MANUFACTURER);
  bledis.setModel(BLUETOOTH_MODEL);
  bledis.setFirmwareRev("0.6.2");
  bledis.setHardwareRev("0.7.3");
  bledis.setSoftwareRev("0.6.2");
  bledis.begin();

  Serial.println("Configuring bleuart");
  bleuart.begin();
  bleuart.bufferTXD(true);

  blebas.begin();
  blebas.write(0);
}

void Bluetooth::startAdvertising(void) {
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addAppearance(BLE_APPEARANCE_CYCLING_POWER_SENSOR);
  Bluefruit.Advertising.addService(cyclingPowerService);
  if(debug) {
    Bluefruit.Advertising.addService(bleuart);
  }
  Bluefruit.ScanResponse.addName();
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);
  Bluefruit.Advertising.setFastTimeout(30);  
  Bluefruit.Advertising.start(0);            
}

void Bluetooth::setupCyclingPowerCharacteristic(void) {
  cyclingPowerService.begin();

  cyclingPowerMeasurementCharacteristic.setProperties(CHR_PROPS_NOTIFY);
  cyclingPowerMeasurementCharacteristic.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  // cyclingPowerMeasurementCharacteristic.setFixedLen(8);
  cyclingPowerMeasurementCharacteristic.setMaxLen(11);
//  cyclingPowerMeasurementCharacteristic.setCccdWriteCallback(cccd_callback);
  cyclingPowerMeasurementCharacteristic.begin();
  cyclingPowerFeatureChararacteristic.setProperties(CHR_PROPS_READ);
  cyclingPowerFeatureChararacteristic.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  cyclingPowerFeatureChararacteristic.setFixedLen(4);
  cyclingPowerFeatureChararacteristic.begin();
  cyclingPowerFeatureChararacteristic.write32(0b00000000000000000000000000001000);

  cyclingPowerLocationChararacteristic.setProperties(CHR_PROPS_READ);
  cyclingPowerLocationChararacteristic.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  cyclingPowerLocationChararacteristic.setFixedLen(1);
  cyclingPowerLocationChararacteristic.begin();
  // cyclingPowerLocationChararacteristic.write8(5); // left crank
  cyclingPowerLocationChararacteristic.write8(0b00000101); // left crank
}

void Bluetooth::logToBluetoothSerial(String data) {
     int len = sizeof(data);
    uint8_t buf[len];
    data.getBytes(buf, len);
    bleuart.write(buf, len);
}
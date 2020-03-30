#ifndef _BLUETOOTH_H_
#define _BLUETOOTH_H_

#include <bluefruit.h>
#include "BLECharacteristic.h"
#include "structs.h"

class Bluetooth {

  public:
    Bluetooth();
    void init(void);
    void startAdvertising(void);
    void setupCyclingPowerCharacteristic(void);
    void sendStructData(bluetoothDebugDataEntry *struct_data);
    void logToBluetoothSerial(String data);

    BLEService cyclingPowerService;
    BLECharacteristic cyclingPowerMeasurementCharacteristic;
    BLECharacteristic cyclingPowerLocationChararacteristic;
    BLECharacteristic cyclingPowerFeatureChararacteristic;
    BLECharacteristic cyclingPowerControlPointChararacteristic;
    
    BLEDis bledis;
    BLEUart bleuart;
    BLEBas blebas;    
    
};

#endif

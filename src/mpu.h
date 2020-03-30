#ifndef _mpu_H_
#define _mpu_H_

#include "bluefruit.h"
#include "MPU9250.h"

class Mpu {
    
    public:
        void init(void);
        void update(void);
        float getPitch();
        boolean detectRotation(int value);
    
    private:
        MPU9250 mpu;
        boolean belowZero = false;
        boolean aboveZero = false;
        int halfRotation = 0;
};

#endif
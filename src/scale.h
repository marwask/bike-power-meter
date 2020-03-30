#ifndef _SCALE_H_
#define _SCALE_H_

#include "HX711.h"

class Scale {
    const int LOADCELL_DOUT_PIN = 7;
    const int LOADCELL_SCK_PIN = 15;
    const int CALIBRATION_RETRIES = 30;

    public:
        void init();
        int read(void);
        void calibrateBaseOffset(void);
        int getBaseOffset(void);
        void setBaseOffset(long);
        boolean isReady(void);
        boolean isCalibrating(void);
        void restartScale(void);

    private:
        HX711 hx711;
        long baseOffset = 1680209;
        boolean calibrating = false;
        long offsets[30];
};

#endif
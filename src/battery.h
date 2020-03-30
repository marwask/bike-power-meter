#define VBAT_MV_PER_LSB   (0.73242188F)

#ifdef NRF52840_XXAA
  #define VBAT_DIVIDER      (0.5F)
  #define VBAT_DIVIDER_COMP (2.0F)
#else
  #define VBAT_DIVIDER      (0.71275837F)
  #define VBAT_DIVIDER_COMP (1.403F)
#endif

#define REAL_VBAT_MV_PER_LSB (VBAT_DIVIDER_COMP * VBAT_MV_PER_LSB)

class Battery {

  const uint32_t vbat_pin = A7;

  public:
    float readVBAT(void);
    uint8_t milliVoltsToPercent(float mvolts);
    float getBatteryPercentage();

};
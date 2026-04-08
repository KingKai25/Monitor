#pragma once
#include <Arduino.h>

// Module cảnh báo cháy, khói
class FireSmokeAlert {
private:
    uint8_t firePin;
    uint8_t smokePin;
public:
    FireSmokeAlert(uint8_t fire, uint8_t smoke);
    void begin();
    bool checkFire();
    bool checkSmoke();
};

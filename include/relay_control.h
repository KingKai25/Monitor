#pragma once
#include <Arduino.h>

// Module điều khiển relay (đèn)
class RelayControl {
private:
    uint8_t relayPin;
public:
    RelayControl(uint8_t pin);
    void begin();
    void set(bool state);
    bool getState();
};

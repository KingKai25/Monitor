#pragma once
#include <Arduino.h>
#include <DHT.h>

// Module đọc cảm biến nhiệt độ, độ ẩm DHT11
class DHTSensor {
private:
    DHT dht;
public:
    DHTSensor(uint8_t pin, uint8_t type);
    void begin();
    float readTemperature();
    float readHumidity();
};
